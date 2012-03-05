
def {
  x: Int
  y: Int
} Coord;

def {
  char: Sym
  color: Sym
} Skin;

def {
  tag: Sym
  skin: Skin
  name: Sym

  walkable 
  visible
  stairs
  sticky
  water
  s_shrine
  b_shrine
  v_shrine
  shootable 
  warm
  healingfountain
  nofeature
  confuse
  lit
  pois2
  bb_shrine
  permanent
  explode 

  poison: Real
  queasy: Real 
  fire: Real

  back: Sym
  resource: Sym
  sign: Sym
  special: Sym
  branch: Sym 

  lightbonus: Int
  height: Int

} Feature;

Feature{ skin=Skin{char='aa' color='light_blue'} 
         stairs=true visible=nil};


