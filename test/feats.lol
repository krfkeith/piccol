

def {
  x: Int
  y: Int
} Coord;

def {
  char: Sym
  color: Sym
} Skin;

def {
  name: Sym

  sticky
  s_shrine
  b_shrine
  v_shrine
  shootable 
  warm
  healingfountain
  confuse
  pois2
  bb_shrine
  permanent
  explode 

  poison: Real
  queasy: Real 
  fire: Real

  resource: Sym
  sign: Sym
  special: Sym
  branch: Sym 

  stairs: Int
  lightbonus: Int

} FeatureFlags;

def {
  lit
  set_water
  clear_water
  height: Int
  walkable 
  visible
} FeatureGridprops;

def {
  flags: FeatureFlags
  props: FeatureGridprops
  back: Sym
} FeatureNoSkin;

def {
  flags: FeatureFlags
  props: FeatureGridprops
  skin: Skin
} Feature;


def { tag: Sym feat:  Feature          } Feattag;
def { tag: Sym props: FeatureGridprops } Featnonetag;
def { tag: Sym feat:  FeatureNoSkin    } Featnoskintag;


floor_gridprops Void->FeatureGridprops :- 
   FeatureGridprops{ lit=false set_water=false clear_water=false walkable=true visible=true height=-10 }.


init_featstock Void -> Void :-

Feattag { tag='>' 
          feat=Feature{ props=(floor_gridprops->FeatureGridprops)
                        skin=Skin{char='>' color='white'} 
                        flags=FeatureFlags{stairs=1 name='a hole in the floor'} }
} set_featstock

Feattag{ tag='1' 
         feat=Feature{ props=(floor_gridprops->FeatureGridprops)
                       skin=Skin{char='>' color='lime'} 
                       flags=FeatureFlags{stairs=1 name='a hole in the floor' branch='a'} }
} set_featstock

Feattag{ tag='2' 
         feat=Feature{ props=(floor_gridprops->FeatureGridprops)
                       skin=Skin{char='>' color='crimson'}
                       flags=FeatureFlags{stairs=1 name='a hole in the floor' branch='b'} }
} set_featstock

/*
Feature{ tag='3' walkable=true visible=true skin=Skin{char='>' color='sky'}
         stairs=1 name='a hole in the floor' branch='c'};

Feature{ tag='4' walkable=true visible=true skin=Skin{char='>' color='dark_gray'}
         stairs=1 name='a hole in the floor' branch='d'};

Feature{ tag='5' walkable=true visible=true skin=Skin{char='>' color='light_gray'}
         stairs=1 name='a hole in the floor' branch='e'};

Feature{ tag='6' walkable=true visible=true 
         skin=Skin{ char=175->Sym color='white'} 
         stairs=1 name='a hole in the floor' branch='s'};

Feature{ tag='8' walkable=true visible=true skin=Skin{char=175->Sym color='red'}
         stairs=1 name='an entrance to the Rehabilitation Thunderdome' branch='q'};

Feature{ tag='qk' walkable=true visible=true skin=Skin{char=175->Sym color='dark_gray'}
         stairs=1 name='an entrace to the temple of Kali' branch='qk'};

Feature{ tag='*' walkable=true visible=nil skin=Skin{char='*' color='lightest_green'}
         name='rubble'};

Feature{ tag='^' walkable=true visible=true skin=Skin{char=248->Sym color='red'}
         sticky=true name='a cave floor covered with glue'};

Feature{ tag='s' walkable=true visible=true skin=Skin{char=234->Sym  color='darker_grey'}
         s_shrine=true name='a shrine to Shiva'};

Feature{ tag='b' walkable=true visible=true skin=Skin{char=127->Sym color='white'}
         b_shrine=true name='a shrine to Brahma'};

Feature{ tag='v' walkable=true visible=true skin=Skin{char=233->Sym color='azure'}
         v_shrine=true name='a shrine to Vishnu'};

Feature{ tag='bb' walkable=true visible=true skin=Skin{char=16->Sym color='crimson'}
         bb_shrine=true name='an Altar of Ba\'al-Zebub'};

Feature{ tag='dd' walkable=true visible=true skin=Skin{char='^' color='white'}
         lit=true lightbonus=7 name='a dolmen'};

Feature{ tag=':' walkable=nil visible=nil skin=Skin{char=9->Sym color='white'}
         name='a column' height=0};

Feature{ tag='h' walkable=true visible=true skin=Skin{char=242->Sym color='white'}
         stairs=6 name='a dropchute'};

Feature{ tag='a' walkable=true visible=true skin=Skin{char=254->Sym color='green'}
         name='an abandoned altar stone'};

Feature{ tag='@' walkable=true visible=true skin=Skin{char=15->Sym color='yellow'}
         name='a hearth' warm=true};

Feature{ tag='$' walkable=true visible=true skin=Skin{char=20->Sym color='light_sky'}
         name='a Fountain of Youth' healingfountain=true};


Feature{ tag='=' walkable=nil visible=true skin=Skin{char=196->Sym color='gray'}
         name='barricades' shootable=true};

Feature{ tag='l' walkable=nil visible=true skin=Skin{char=179->Sym color='gray'}
         name='barricades' shootable=true};

Feature{ tag='r' walkable=nil visible=true skin=Skin{char=218->Sym color='gray'}
         name='barricades' shootable=true};

Feature{ tag='q' walkable=nil visible=true skin=Skin{char=191->Sym color='gray'}
         name='barricades' shootable=true};

Feature{ tag='p' walkable=nil visible=true skin=Skin{char=192->Sym color='gray'}
         name='barricades' shootable=true};

Feature{ tag='d' walkable=nil visible=true skin=Skin{char=217->Sym color='gray'}
         name='barricades' shootable=true};


Feature{ tag='|' walkable=nil visible=nil skin=Skin{char=186->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='-' walkable=nil visible=nil skin=Skin{char=205->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='+' walkable=nil visible=nil skin=Skin{char=206->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='R' walkable=nil visible=nil skin=Skin{char=201->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='L' walkable=nil visible=nil skin=Skin{char=200->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='T' walkable=nil visible=nil skin=Skin{char=203->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='F' walkable=nil visible=nil skin=Skin{char=204->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='J' walkable=nil visible=nil skin=Skin{char=202->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='7' walkable=nil visible=nil skin=Skin{char=187->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='/' walkable=nil visible=nil skin=Skin{char=188->Sym color='white'}
         name='a smooth stone wall' height=0};

Feature{ tag='Z' walkable=nil visible=nil skin=Skin{char=185->Sym color='white'}
         name='a smooth stone wall' height=0};


Feature{ tag='|.' walkable=nil visible=true skin=Skin{char=186->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='-.' walkable=nil visible=true skin=Skin{char=205->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='+.' walkable=nil visible=true skin=Skin{char=206->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='R.' walkable=nil visible=true skin=Skin{char=201->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='L.' walkable=nil visible=true skin=Skin{char=200->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='T.' walkable=nil visible=true skin=Skin{char=203->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='F.' walkable=nil visible=true skin=Skin{char=204->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='J.' walkable=nil visible=true skin=Skin{char=202->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='7.' walkable=nil visible=true skin=Skin{char=187->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='/.' walkable=nil visible=true skin=Skin{char=188->Sym color='gray'}
         name='bulletproof glass' height=0};

Feature{ tag='Z.' walkable=nil visible=true skin=Skin{char=185->Sym color='gray'}
         name='bulletproof glass' height=0};



Feature{ tag='Y' walkable=nil visible=nil skin=Skin{char=157->Sym color='green'}
         name='a tree' height=5};

Feature{ tag='!' walkable=true visible=nil skin=Skin{char=173->Sym color='dark_green'}
         name='a giant fern'};

Feature{ tag='!f' walkable=true visible=true skin=Skin{char=24->Sym color='desaturated_green'}
         name='a flowering fern'};

Feature{ tag='w' walkable=true visible=true skin=Skin{char='-' color='sky'}
         water=1 name='a pool of water'};

*/

Featnonetag{ tag='W' 
             props=FeatureGridprops{ walkable=true visible=true set_water=true lit=false 
                                     clear_water=false height=-10 }
} set_featstock

Featnonetag{ tag='.' 
             props=FeatureGridprops{ walkable=true visible=true set_water=false lit=false 
                                     clear_water=true height=-10 }
} set_featstock


Featnoskintag{ tag='e' 
               feat=FeatureNoSkin{ props=(floor_gridprops->FeatureGridprops)
                                   back='desaturated_green'
                                   flags=FeatureFlags{ poison=0.5 name='a cloud of Ebola virus' } }
} set_featstock

Featnoskintag{ tag='f' 
               feat=FeatureNoSkin{ props=(floor_gridprops->FeatureGridprops)
                                   back='gray'
                                   flags=FeatureFlags{ confuse=true name='confusing smoke' } }
} set_featstock

/*
Feature{ tag='g' walkable=true visible=true skin=Skin{} back='dark_green'
         poison=0.25 pois2=true name='spores of black mold'};

Feature{ tag='&' walkable=true visible=true skin=Skin{} back='darkest_blue'
         lit=true queasy=0.1 name='swamp gas' explode=true};

Feature{ tag='"' walkable=true visible=true skin=Skin{} back='red'
         fire=0.333 name='napalm'};


Feature{ tag='C' walkable=true visible=true skin=Skin{char=20->Sym color='dark_green'}
         resource='g' name='a Green Fountain'};

Feature{ tag='V' walkable=true visible=true skin=Skin{char=20->Sym color='dark_red'} 
         resource='r' name='a Red Fountain'};

Feature{ tag='B' walkable=true visible=true skin=Skin{char=20->Sym color='dark_yellow'}
         resource='y' name='a Yellow Fountain'};

Feature{ tag='N' walkable=true visible=true skin=Skin{char=20->Sym color='dark_sky'}
         resource='b' name='a Blue Fountain'};

Feature{ tag='M' walkable=true visible=true skin=Skin{char=20->Sym color='dark_purple'} 
         resource='p' name='a Purple Fountain'};


# quest/winning specials

Feature{ tag='signkali' walkable=true visible=true skin=Skin{char='.' color='white'}
         sign='kali ma kali ma shakti de' 
         name='an engraving on the floor'};

Feature{ tag='kali' walkable=true visible=true skin=Skin{char=234->Sym  color='white'}
         special='kali' name='a statue of Kali'};

Feature{ tag='signvault' walkable=true visible=true skin=Skin{char=254->Sym color='white'}
         sign='"Entrance to the Vault. Robbers beware."'
         name='an engraving on the floor'};

Feature{ tag='##' walkable=nil visible=nil skin=Skin{char=176->Sym color='grey'}
         name='carbonized graphite' height=1};

Feature{ tag='#!' walkable=nil visible=nil skin=Skin{char=176->Sym color='grey'}
         name='crystalline graphite' height=1000 permanent=true};

Feature{ tag='cthulhu' walkable=true visible=true skin=Skin{char=16->Sym color='gray'}
         special='cthulhu' name='an unholy stone'};

Feature{ tag='signcth1' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='PH\'NGLUI'
         name='eldritch engravings'};

Feature{ tag='signcth2' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='MGLW\'NAFH'
         name='eldritch engravings'};

Feature{ tag='signcth3' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='CTHULHU'
         name='eldritch engravings'};

Feature{ tag='signcth4' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='R\'LYEH'
         name='eldritch engravings'};

Feature{ tag='signcth5' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='WGAH\'NAGL'
         name='eldritch engravings'};

Feature{ tag='signcth6' walkable=true visible=true skin=Skin{char=250->Sym color='red'}
         sign='FHTAGN'
         name='eldritch engravings'};

Feature{ tag='monolith' walkable=true visible=true skin=Skin{char=8->Sym color='light_gray'}
         special='monolith' name='the Monolith' };

*/

.

def { xy:Coord tag:Sym } XYTag;

def { xy:Coord feat:Feature } XYFeature;
def { xy:Coord feat:FeatureNoSkin } XYFeatureNoSkin;
def { xy:Coord props:FeatureGridprops } XYFeatureGridprops;
def { xy:Coord skin:Skin} XYSkin;

set_feature XYTag -> Void :- 
  XYFeature{xy=.xy feat=(Tag{tag=.tag} get_featstock->Feature)} set_feature;
  XYFeatureNoSkin{xy=.xy feat=(Tag{tag=.tag} get_featstock->FeatureNoSkin)} set_feature;
  XYFeatureGridprops{xy=.xy props=(Tag{tag=.tag} get_featstock->FeatureGridprops)} set_feature.

set_feature XYFeature -> Void :- 
  XYFeatureGridprops{xy=.xy props=.feat.props} set_feature
  XYSkin{xy=.xy skin=.feat.skin} set_skin
  XYFeatureFlags{xy=.xy flags=.feat.flags} set_featmap.

set_feature XYFeatureNoSkin -> Void :- .
  XYFeatureGridprops{xy=.xy props=.feat.props} set_feature


set_feature XYFeatureGridprops -> Void :- .


/*

set_gridprops GridProps->Void callback;

add_featmap Featpoint->Void callback;
del_featmap Coord->Void callback;
get_featmap Coord->Feature callback;
in_featmap  Coord->Bool callback;

get_featstock Tag->Feature callback;
set_featstock Featsym->Void callback;



: set_feature [Featpoint->Void] {
  GridProps{xy=$xy walkable=$feat.walkable height=$feat.height water=$feat.water->Bool} set_gridprops 
  $feat.nofeature ? { Coord{xy=$xy} del_featmap }
                    { add_featmap }
}


: set_featsym [Tagpoint->Void] {
Featpoint{xy=$xy feat=Tag{tag=$tag} get_featstock} set_feature
}

set_renderprops Coord XY -> Void :-

[ get_featmap -> Feature FEAT, 
  XyBool{xy=XY on=FEAT.lit} set_is_lit, 
  XySym [ FEAT.back->Sym, {xy=XY sym=FEAT.back} ]
        [ {xy=XY sym='black'} ] set_back


        x, y = xy

        feat = None
        if xy in self.d.featmap:
            feat = self.d.featmap[xy]

        if feat and feat.lit:
            dg.render_set_is_lit(x, y, True)
        else:
            dg.render_set_is_lit(x, y, False)

        if feat and feat.back:
            dg.render_set_back(x, y, feat.back)
        else:
            dg.render_set_back(x, y, libtcod.black)

        fore = self.theme[self.d.branch][0]
        fore2 = fore
        fore_i = 0
        is_terrain = False
        c = ' '

        walkable = dg.grid_is_walk(x, y)
        if feat and feat.skin:
            c, fore = feat.skin

        elif walkable:
            if dg.grid_is_water(x, y):
                c = 251
                fore = libtcod.light_azure
                fore2 = libtcod.dark_azure
                fore_i = 1
            else:
                c = 250
                is_terrain = True

        else:
            if dg.grid_is_water(x,y):
                fore = libtcod.desaturated_blue
            c = 176
            is_terrain = True

        dg.render_set_skin(x, y, fore, c, fore2, fore_i, is_terrain)

        ## 
        if feat:
            dg.render_set_is_viewblock(x, y, not feat.visible, 0)
            dg.render_set_is_walkblock(x, y, not feat.walkable, 0)
        else:
            dg.render_set_is_viewblock(x, y, not walkable, 0)
            dg.render_set_is_walkblock(x, y, not walkable, 0)



*/
