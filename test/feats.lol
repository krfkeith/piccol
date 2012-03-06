
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
  sticky
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

  water: Int
  stairs: Int
  lightbonus: Int
  height: Int

} Feature;

 
Feature{ tag='>' walkable=true visible=true skin=Skin{char='>' color='white'} 
         stairs=1 name='a hole in the floor'};

Feature{ tag='1' walkable=true visible=true skin=Skin{char='>' color='lime'} 
         stairs=1 name='a hole in the floor' branch='a'};

Feature{ tag='2' walkable=true visible=true skin=Skin{char='>' color='crimson'}
         stairs=1 name='a hole in the floor' branch='b'};

Feature{ tag='3' walkable=true visible=true skin=Skin{char='>' color='sky'}
         stairs=1 name='a hole in the floor' branch='c'};

Feature{ tag='4' walkable=true visible=true skin=Skin{char='>' color='dark_gray'}
         stairs=1 name='a hole in the floor' branch='d'};

Feature{ tag='5' walkable=true visible=true skin=Skin{char='>' color='light_gray'}
         stairs=1 name='a hole in the floor' branch='e'};

Feature{ tag='6' walkable=true visible=true 
         skin=Skin{ char='175' color='white'} 
         stairs=1 name='a hole in the floor' branch='s'};

Feature{ tag='8' walkable=true visible=true skin=Skin{char='175' color='red'}
         stairs=1 name='an entrance to the Rehabilitation Thunderdome' branch='q'};

Feature{ tag='qk' walkable=true visible=true skin=Skin{char='175' color='dark_gray'}
         stairs=1 name='an entrace to the temple of Kali' branch='qk'};

Feature{ tag='*' walkable=true visible=nil skin=Skin{char='*' color='lightest_green'}
         name='rubble'};

Feature{ tag='^' walkable=true visible=true skin=Skin{char='248' color='red'}
         sticky=true name='a cave floor covered with glue'};

Feature{ tag='s' walkable=true visible=true skin=Skin{char='234'  color='darker_grey'}
         s_shrine=true name='a shrine to Shiva'};

Feature{ tag='b' walkable=true visible=true skin=Skin{char='127' color='white'}
         b_shrine=true name='a shrine to Brahma'};

Feature{ tag='v' walkable=true visible=true skin=Skin{char='233' color='azure'}
         v_shrine=true name='a shrine to Vishnu'};

Feature{ tag='bb' walkable=true visible=true skin=Skin{char='16' color='crimson'}
         bb_shrine=true name='an Altar of Ba\'al-Zebub'};

Feature{ tag='dd' walkable=true visible=true skin=Skin{char='^' color='white'}
         lit=true lightbonus=7 name='a dolmen'};

Feature{ tag=':' walkable=nil visible=nil skin=Skin{char='9' color='white'}
         name='a column' height=0};

Feature{ tag='h' walkable=true visible=true skin=Skin{char='242' color='white'}
         stairs=6 name='a dropchute'};

Feature{ tag='a' walkable=true visible=true skin=Skin{char='254' color='green'}
         name='an abandoned altar stone'};

Feature{ tag='@' walkable=true visible=true skin=Skin{char='15' color='yellow'}
         name='a hearth' warm=true};

Feature{ tag='$' walkable=true visible=true skin=Skin{char='20' color='light_sky'}
         name='a Fountain of Youth' healingfountain=true};


Feature{ tag='=' walkable=nil visible=true skin=Skin{char='196' color='gray'}
         name='barricades' shootable=true};

Feature{ tag='l' walkable=nil visible=true skin=Skin{char='179' color='gray'}
         name='barricades' shootable=true};

Feature{ tag='r' walkable=nil visible=true skin=Skin{char='218' color='gray'}
         name='barricades' shootable=true};

Feature{ tag='q' walkable=nil visible=true skin=Skin{char='191' color='gray'}
         name='barricades' shootable=true};

Feature{ tag='p' walkable=nil visible=true skin=Skin{char='192' color='gray'}
         name='barricades' shootable=true};

Feature{ tag='d' walkable=nil visible=true skin=Skin{char='217' color='gray'}
         name='barricades' shootable=true};


Feature{ tag='|' walkable=nil visible=nil skin=Skin{char='186' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='-' walkable=nil visible=nil skin=Skin{char='205' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='+' walkable=nil visible=nil skin=Skin{char='206' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='R' walkable=nil visible=nil skin=Skin{char='201' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='L' walkable=nil visible=nil skin=Skin{char='200' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='T' walkable=nil visible=nil skin=Skin{char='203' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='F' walkable=nil visible=nil skin=Skin{char='204' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='J' walkable=nil visible=nil skin=Skin{char='202' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='7' walkable=nil visible=nil skin=Skin{char='187' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='/' walkable=nil visible=nil skin=Skin{char='188' color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='Z' walkable=nil visible=nil skin=Skin{char='185' color='white'}
         name='a smooth stone wall' height=0};


Feature{ tag='|.' walkable=nil visible=true skin=Skin{char='186' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='-.' walkable=nil visible=true skin=Skin{char='205' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='+.' walkable=nil visible=true skin=Skin{char='206' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='R.' walkable=nil visible=true skin=Skin{char='201' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='L.' walkable=nil visible=true skin=Skin{char='200' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='T.' walkable=nil visible=true skin=Skin{char='203' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='F.' walkable=nil visible=true skin=Skin{char='204' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='J.' walkable=nil visible=true skin=Skin{char='202' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='7.' walkable=nil visible=true skin=Skin{char='187' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='/.' walkable=nil visible=true skin=Skin{char='188' color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='Z.' walkable=nil visible=true skin=Skin{char='185' color='gray'}
         name='bulletproof glass' height=0};



Feature{ tag='Y' walkable=nil visible=nil skin=Skin{char='157' color='green'}
         name='a tree' height=5};

Feature{ tag='!' walkable=true visible=nil skin=Skin{char='173' color='dark_green'}
         name='a giant fern'};

Feature{ tag='!f' walkable=true visible=true skin=Skin{char='24' color='desaturated_green'}
         name='a flowering fern'};

Feature{ tag='w' walkable=true visible=true skin=Skin{char='-' color='sky'}
         water=1 name='a pool of water'};

Feature{ tag='W' walkable=true visible=true nofeature=true 
         water=1};

Feature{ tag='.' walkable=true visible=true nofeature=true 
         water=-1};

Feature{ tag='e' walkable=true visible=true skin=Skin{} back='desaturated_green'
         poison=0.5 name='a cloud of Ebola virus'};

Feature{ tag='f' walkable=true visible=true skin=Skin{} back='gray'
         confuse=true name='confusing smoke'};

Feature{ tag='g' walkable=true visible=true skin=Skin{} back='dark_green'
         poison=0.25 pois2=true name='spores of black mold'};

Feature{ tag='&' walkable=true visible=true skin=Skin{} back='darkest_blue'
         lit=true queasy=0.1 name='swamp gas' explode=true};

Feature{ tag='"' walkable=true visible=true skin=Skin{} back='red'
         fire=0.333 name='napalm'};


Feature{ tag='C' walkable=true visible=true skin=Skin{char='20' color='dark_green'}
         resource='g' name='a Green Fountain'};

Feature{ tag='V' walkable=true visible=true skin=Skin{char='20' color='dark_red'} 
         resource='r' name='a Red Fountain'};

Feature{ tag='B' walkable=true visible=true skin=Skin{char='20' color='dark_yellow'}
         resource='y' name='a Yellow Fountain'};

Feature{ tag='N' walkable=true visible=true skin=Skin{char='20' color='dark_sky'}
         resource='b' name='a Blue Fountain'};

Feature{ tag='M' walkable=true visible=true skin=Skin{char='20' color='dark_purple'} 
         resource='p' name='a Purple Fountain'};


# quest/winning specials

Feature{ tag='signkali' walkable=true visible=true skin=Skin{char='.' color='white'}
         sign='kali ma kali ma shakti de' 
         name='an engraving on the floor'};

Feature{ tag='kali' walkable=true visible=true skin=Skin{char='234'  color='white'}
         special='kali' name='a statue of Kali'};

Feature{ tag='signvault' walkable=true visible=true skin=Skin{char='254' color='white'}
         sign='"Entrance to the Vault. Robbers beware."'
         name='an engraving on the floor'};

Feature{ tag='##' walkable=nil visible=nil skin=Skin{char='176' color='grey'}
         name='carbonized graphite' height=1};

Feature{ tag='#!' walkable=nil visible=nil skin=Skin{char='176' color='grey'}
         name='crystalline graphite' height=1000 permanent=true};

Feature{ tag='cthulhu' walkable=true visible=true skin=Skin{char='16' color='gray'}
         special='cthulhu' name='an unholy stone'};

Feature{ tag='signcth1' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='PH\'NGLUI'
         name='eldritch engravings'};

Feature{ tag='signcth2' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='MGLW\'NAFH'
         name='eldritch engravings'};

Feature{ tag='signcth3' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='CTHULHU'
         name='eldritch engravings'};

Feature{ tag='signcth4' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='R\'LYEH'
         name='eldritch engravings'};

Feature{ tag='signcth5' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='WGAH\'NAGL'
         name='eldritch engravings'};

Feature{ tag='signcth6' walkable=true visible=true skin=Skin{char='250' color='red'}
         sign='FHTAGN'
         name='eldritch engravings'};

Feature{ tag='monolith' walkable=true visible=true skin=Skin{char='8' color='light_gray'}
         special='monolith' name='the Monolith'};




