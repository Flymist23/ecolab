#include <oldarrays.h>

class StupidBug
{
  GraphID_t cellID;  // Graphcode ID of cell where this bug is located
  CLASSDESC_ACCESS(StupidBug);
  friend class Cell;
public:
  int x(); 
  int y(); 
  double size;
  double repro_size;
  double max_consumption;
  double survivalProbability;
  StupidBug(): size(1), repro_size(10), max_consumption(1), survivalProbability(0.98) {}
  void move();
  void grow(); 
  void die();
  bool mortality();
  void draw(const eco_string& canvas);
};

class Predator
{
  GraphID_t cellID;  // Graphcode ID of cell where this bug is located
  CLASSDESC_ACCESS(Predator);
  friend class Cell;
  void move(Cell& to, Cell& from);
public:
  int x(); 
  int y(); 
  void hunt();
  void draw(const eco_string& canvas);
};


class Cell: public object
{
  CLASSDESC_ACCESS(Cell);
public:
  unsigned x,y;
  double food_avail, food_production, max_food;
  vector<ref<StupidBug> > bug;
  Ptrlist bugNbrhd; 
  vector<ref<Predator> > predator;
  Ptrlist predatorNbrhd;
  bool occupied() {return bug.size()>0;}
  Cell() {}
  Cell(unsigned x_, unsigned y_, double mfp=0.01):
    x(x_), y(y_), food_avail(0), food_production(mfp) {}
  Cell(const StupidBug& b) {bug.push_back(b);}
  void addBug(const StupidBug& b) {
    assert(!occupied()); bug.resize(1); assert(!bug[0]); *bug[0]=b;
    bug[0]->cellID=begin()->ID; 
  }
  void addPredator(const Predator& b) {
    predator.resize(1); *predator[0]=b;
    predator[0]->cellID=begin()->ID; 
  }
  void moveBug(Cell& from) { 
    assert(from.size()>0);
    assert(!occupied());
    if (&from==this) return;
    bug=from.bug; 
    from.bug.clear(); 
  }
  
  void grow_food() {food_avail+=food_production;}
  void Cell::draw(const eco_string& canvas);

  /* override virtual methods of object */
  void lpack(pack_t *buf);
  void lunpack(pack_t *buf);
  object* lnew() const {return vnew(this);}
  object* lcopy() const {return vcopy(this);}
  int type() const {return vtype(*this);}
};

/* casting utilities */
inline Cell* getCell(objref& x) {assert(x->size()==0 || x.ID==(*x)[0].ID); return dynamic_cast<Cell*>(&*x);}
inline const Cell* getCell(const objref& x) {return dynamic_cast<const Cell*>(&*x);}

/* casting utilities */
inline StupidBug* getBug(objref& x)   {return dynamic_cast<StupidBug*>(&*x);}
inline const StupidBug* getBug(const objref& x)   {return dynamic_cast<const StupidBug*>(&*x);}

class Space: public Graph
{
  bool toroidal;
  CLASSDESC_ACCESS(Space);
 public:
  int nx, ny;  //dimensions of space
  urand u;
  Space(): nx(0), ny(0) {}
  void setup(int nx, int ny, int moveDistance, bool toroidal, 
	     double max_food_production);
  GraphID_t mapid(int x, int y);
  objref* getObjAt(int x, int y) {return &objects[mapid(x,y)];}
  Cell* randomCell() {
    objref *o;
    while ( (o=getObjAt(u.rand()*nx,u.rand()*ny))->proc!=myid );
    return getCell(*o);
  }
};

namespace std
{
  template <class T>
  struct less<ref<T> >
  { 
    bool operator()(const ref<T>& x, const ref<T>& y) const
    {
      return &*x<&*y;
    }
  };
}

template <class T>
bool unique(const vector<ref<T> > x)
{
  set<ref<T> > s;
  for (typename vector<ref<T> >::const_iterator i=x.begin(); i!=x.end(); i++)
    if (*i && s.count(*i))
      return false;
    else
      s.insert(*i);
  return true;
}

class StupidModel: public Space, TCL_obj_t
{
  CLASSDESC_ACCESS(StupidModel);
public:
  urand u;     //random generator for positions
  int tstep;   //timestep - updated each time moveBugs is called
  int scale;   //no. pixels used to represent bugs
  vector<ref<StupidBug> > bugs; 
  vector<ref<Predator> > predators; 
  random_gen *initBugDist; //Initial distribution of bug sizes
  vector<vector< pair<GraphID_t,GraphID_t> > > emmigration_list;

  StupidModel(): initBugDist(&u) {}
  void setup(TCL_args args) {
    parallel(args);
    int nx=args, ny=args, moveDistance=args;
    bool toroidal=args;
    double max_food_production=args;
    Space::setup(nx,ny,moveDistance,toroidal,max_food_production);
  }
  // addBugs(int nBugs);
  void addBugs(TCL_args args);
  void addPredators(TCL_args args);
  void moveBugs(TCL_args);
  void birthdeath(TCL_args);
  void drawBugs(TCL_args args);
  void drawPredators(TCL_args args);
  void drawCells(TCL_args args);
  void grow(TCL_args);
  void killBug(ref<StupidBug>& bug);
  void hunt(TCL_args);

  /** return a TCL object representing a bug 
      (if one exists at that location, and bug passed as third paramter) */
  eco_string probe(TCL_args); 
  array bugsizes() {
    array r;
    for (int i=0; i<bugs.size(); i++)
      r <<= bugs[i]->size;
    return r;
  }
  double max_bugsize(TCL_args args) {
    parallel(args);
    double r=-1;
    for (int i=0; i<bugs.size(); i++)
      r = std::max(bugs[i]->size,r);
#ifdef MPI_SUPPORT
    double r1;
    MPI_Reduce(&r,&r1,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    return r1;
#else
    return r;
#endif
  }
  void read_food_production(TCL_args);
};
