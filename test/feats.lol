
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
  walkable
  visible
  stairs
} Feature;

Feature{ skin=Skin{char='aa' color='light_blue'} 
         stairs=true visible=nil};


