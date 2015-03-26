#38000
лезть на ель~
2 c 0
лезть залезть~
if !(%arg.contains(ель)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Хватаясь за колючие ветки, вы полезли вверх.
wechoaround %actor% %actor.name% полез%actor.q% вверх, хватаясь за ветки.
wait 1s
wsend %actor% .- Вы на верхушке дерева.
wteleport %actor% 38097
%actor.wait(1)%
wat 38097  wechoaround %actor% Кто-то пролез сюда снизу.
~
#38001
прыгнуть с ели~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
switch %random.3%
  case 1
    wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Осторожней!
    wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
    wait 1s
    wsend %actor% .- Вы плюхнулись на лужайку перед деревом.
    wteleport %actor.name% 38021
    wat 38021 wechoaround %actor% Кто-то спрыгнул с ели. Во дела!
  break
  case 2
    wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Осторожней!
    wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
    wait 1s
    wsend %actor% .- Вы плюхнулись на лужайку перед деревом.
    wteleport %actor.name% 38005
    wat 38005 wechoaround %actor% Кто-то спрыгнул с ели. Во дела!
  break
  case 3
    wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Осторожней!
    wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
    wait 1s
    wsend %actor% .- Вы плюхнулись на лужайку перед деревом.
    wteleport %actor.name% 38009
    wsend %actor% _Вы неудачно приземлились и поранили ногу.
    wdamage %actor% 9
    wat 38009 wechoaround %actor% Кто-то спрыгнул с ели. Во дела!
  break
  default
  break
done
end
~
#38002
даеш бабки богатырю~
0 m 1
~
wait 1
if %amount% < 10 then
  хох
  say За такую получку ночуй под воротами...
  halt
end
wait 1
ул
say Проходите, гости дорогие!
mecho Богатырь отворил ворота.
mdoor 38080 e purge
mdoor 38080 e room 38083
mdoor 38080 e flags a
mdoor 38080 e name ворота
mdoor 38080 e description ворота
mdoor 38080 e key 38000
mdoor 38083 w purge
mdoor 38083 w room 38080
mdoor 38083 w flags a
mdoor 38083 w name ворота
mdoor 38083 w description ворота
mdoor 38083 w key 38000
wait 5s
mdoor 38080 e purge
mdoor 38080 e room 38083
mdoor 38080 e flags abc
mdoor 38080 e name ворота
mdoor 38080 e description ворота
mdoor 38080 e key 38000
mdoor 38083 w purge
mdoor 38083 w room 38080
mdoor 38083 w flags ab
mdoor 38083 w name ворота
mdoor 38083 w description ворота
mdoor 38083 w key 38000
mecho Богатырь затворил ворота.
~
#38003
пришел скиталец~
0 g 100
~
if %actor.vnum% == 38017
  wait 1
  mecho Скиталец попросился на ночлег.
  wait 1
  mecho Богатырь не сжалился над ним и указал ему пальцем в сторону зимующих раков.
end
~
#38004
померла одноглазка~
0 f 100
~
*if (%world.curobjs(615)% < 40) && (%random.3% == 1)
*   mload obj 615
*end
~
#38005
померла двуглазка~
0 f 100
~
*if (%world.curobjs(602)% < 40) && (%random.3% == 1)
*   mload obj 602
*end
~
#38006
померла трехглазка~
0 f 100
~
*if (%world.curobjs(600)% < 40) && (%random.3% == 1)
*   mload obj 600
*end
~
#38007
помер богатырь~
0 f 100
~
if %world.curobjs(38000)% < 1
  mload obj 38000
end
~
#38008
Померла злая старуха~
0 f 100
~
if ((%world.curobjs(598)% < 1 ) && (%random.1000% <= 140 ))
  mload obj 598
end
~
$~
