#33800
у горбуна с дамаджем~
0 p 100
~
wait 2s
say _Хех...нет... силушкой меряться я  с тобой не буду. 
дум
say Ну разве что за 100 кун... али больше...
calcuid gorbj 33831 mob
attach 33801 %gorbj.id% 
exec  33801 %gorbj.id% 
~
#33801
дал горбуну более ста кун~
0 z 100
~
say _Хотя... Можно и тряхнуть силушкой, вспомнить былые времена ратные.
wait 1
вста
mecho _Горбун засучил рукава и кинулся в бой!
calcuid gorbun 33831 mob
detach 33800 %gorbun.id%
mkill %actor%
calcuid gorbun 33831 mob
detach 33801 %gorbun.id%
~
#33802
репоп тригеров в остроге~
2 f 100
~
calcuid gorbun 33831 mob
attach 33800 %gorbun.id%
calcuid gorbun 33831 mob
detach 33804 %gorbun.id%
calcuid znachar 33808 mob
detach 33807 %znachar.id%
calcuid rorik 33800 mob
attach 33813 %rorik.id%
calcuid znachar 33808 mob
attach 33806 %znachar.id%
calcuid piosik 33827 mob
attach 33805 %piosik.id%
if %exist.mob(33830)%
  calcuid swiach 33830 mob
  detach 33812 %swiach.id%
end
calcuid woewoda 33800 mob
detach 33814 %woewoda.id%
calcuid woewodroomo 33833 room
detach 33815 %woewodroomo.id%
calcuid chanor 33820 mob
attach 33824 %chanor.id%
~
#33803
в бою с горбуном~
0 l 100
~
wait 1
if %self.hitp% < 200
  say _Ну вот... Не, драться я больше не хочу - пойду отдыхать.
  calcuid gorbun 33831 mob
  attach 33804 %gorbun.id%
  wait 1
  say _Крепко дерешься ты, право слово...
end
~
#33804
у горбуна после боя~
0 p 100
~
wait 1
say _Нынче я не дерусь... Отдыхаю.
mecho Горбун вздохнул устало и ушел.
mpurge %self%
~
#33805
в бою с псом~
0 r 100
~
wait 3s
пла
say _Смилуйся надо мной, %actor.name%!!!
say _Жить не могу так больше... Заколдовал меня злобный знахарь
say _А был я славный слуга Господа нашего бога....
say _Поставил меня здесь сторожить все и злобу мне дал неописуемую...
say _Расколдуй меня.. помоги... Христом богом прошу...Не убивай...
пла
mkill %actor.name%
calcuid znachar 33808 mob
attach 33807 %znachar.id%
calcuid znachar 33808 mob
detach 33806 %znachar.id%
calcuid piosik 33827 mob
detach 33805 %piosik.id%
~
#33806
помер знахарь 1~
0 f 100
~
if (%world.curobjs(524)% == 0) && (%random.5% == 1)
  *mload obj 524
end
if %world.curobjs(33821)% < 2
  mload obj 33821
end
~
#33807
помер знахарь 2~
0 f 100
~
if %world.curobjs(33821)% < 2
  mload obj 33821
end
if %world.curobjs(33822)% < 2
  mload obj 33822
end
~
#33808
дать порошок псу~
0 j 100
~
wait 1
if %object.vnum% == 33821 then
  mecho _Пес схватил порошок зубами и попытался проглотить его.
  mecho _Все вокруг окутал серый дым.
  wait 1
  mpurge порошок
  calcuid roompiosi 33893 room
  attach 33809 %roompiosi.id%
  exec 33809 %roompiosi.id%
  mpurge %self.name%
end
if %object.vnum% == 33822 then
  mecho _Пес схватил порошок зубами и попытался проглотить его.
  mecho _Все вокруг окутал белый дым.
  wait 1
  mpurge порошок
  calcuid roompiosii 33893 room
  attach 33810 %roompiosii.id%
  exec 33810 %roompiosii.id%
  mpurge %self.name%
end
if %object.vnum% == 33823 then
  mecho _Пес схватил порошок зубами и попытался проглотить его.
  mecho _Все вокруг окутал черный дым.
  wait 1
  calcuid varbleiii 33823 obj
  mpurge порошок
  wait 1
  calcuid roompiosuu 33893 room
  attach 33822 %roompiosuu.id%
  exec 33822 %roompiosuu.id%
  mpurge %self.name%
end
~
#33809
лоадим ларву~
2 z 100
~
wait 1
wecho _Внезапно черный пес превратился в мокрую лягушку.
wload mob 33828
wait 3s
wecho _"Ну ты и балда... Ты все попутал%actor.q%." - проквакала лягушка.
calcuid roompios 33893 room
detach 33809 %roompios.id%
~
#33810
лоадим осла~
2 z 100
~
wait 1
wecho  _Внезапно черный пес превратился в длинноухого осла... Смешно.
wload mob 33829
wait 3s
wecho  _"И-а. И-а. Спасибочки. Век не забуду твоей доброты..." - промычал осел.
calcuid roompios 33893 room
detach 33810 %roompios.id%
~
#33811
объединить серый и белые порошки~
2 c 1
объединить мешать замешать соединить~
if !(%arg.contains(порошки)%) 
  wsend       %actor% Что же вы хотите объединить???
  return 0
  halt
end
if !%actor.haveobj(33821)% || !%actor.haveobj(33822)%
  wsend       %actor% Нельзя смешать что-то, если этого у вас нет.
  return 0
  halt
end
wsend       %actor% Вы насыпали в руку немного белого и немного серого порошков.
wechoaround %actor% %actor.name% начал%actor.g% объединять белый и серый порошки.
calcuid vai 33822 obj
wpurge %vai%
calcuid var 33821 obj
wpurge %var%
wait 1s
calcuid piosik 33827 mob
wload obj 33823
wecho  Поднялся черный дым, а когда он рассеялся, вы увидели черный порошок.
~
#33812
священник благодарит и говорит и уходит~
0 z 100
~
wait 3s
say _Ну спасибо. Долго меня держал мерзкий знахарь здесь.
say _Не любит он истинную веру. Ну и бог ему судья.
say _Я пойду к воеводе, все ему расскажу. Теперь он будет тебе доверять.
mecho  Священник быстро ушел куда-то.
calcuid rorik 33800 mob
attach 33814 %rorik.id%
calcuid woewodro 33833 room
attach 33815 %woewodro.id%
wait 1
mpurge %self%
~
#33813
пришел к воеводе~
0 q 100
~
wait 2s
msend       %actor% Воевода недоверчиво взглянул на вас и продолжил свои дела.
mechoaround %actor% Воевода недоверчиво осмотрел %actor.vname%.
calcuid rorikk 33800 mob
detach 33813 %rorikk.id%
~
#33814
воевода дает хоругвь~
0 q 100
~
wait 6s
msend %actor%  Воевода внимательно взглянул на вас.
mechoaround %actor% Воевода одарил %actor.vname% взглядом.
if (%world.curobjs(33800)% < 1) & (%random.15% == 1)
  say Священник много говорил мне про тебя.
  say У меня есть к тебе поручение.
  say Необходимо срочно доставить святую хоругвь в полки.
  say Я тебе ее дам а ты смотри передай кому надо - самому князю.
  say Он  в долгу не останется...
  wait 1s
  mload obj 33800
  дать хоруг %actor.name%
  detach 33814 %self.id%
elseif ( %world.curobjs(3306)% < 1 ) & (%random.10%  == 1 )
  mload obj 3306
  say Священник много говорил мне про тебя.
  say И, я думаю, ты достоин доверия.
  wait 1s
  дать амул %actor.name%
  detach 33814 %self.id%
end
~
#33815
лоад священника у воеводы~
2 g 100
~
wait 1
if %world.curmobs(33800)% == 1
  wait 2s
  wload mob 33832
  wecho _Радостный священник прибежал сюда и начал что то шептать воеводе.
  calcuid woewodroomo 33833 room
  detach 33815 %woewodroomo.id%
end
~
#33816
лезеш в яму с медведем~
2 c 0
лезть пролезть залезть прыгнуть~
if !(%arg.contains(яма)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Подняв решетку вы прыгнули в яму.
wechoaround %actor% %actor.name% прыгнул%actor.g% в яму.
wait 1s
wsend %actor% .- Вы почувствовали запах гнили.
wteleport %actor.name% 33884
wat 33884 wechoaround %actor% Кто-то спрыгнул сюда сверху.
~
#33817
вылезаеш из ямы с медведем~
2 c 0
лезть пролезть залезть прыгнуть~
if !(%arg.contains(вверх)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Подняв решетку вы пролезли из ямы.
wechoaround %actor% %actor.name% полез%actor.q% вверх из ямы.
wait 1s
wsend %actor% .- Вы вылезли из ямы.
wteleport %actor.name% 33893
wechoaround %actor% Кто-то пролез сюда из ямы.
~
#33818
вылезаеш из ямы со змеями~
2 c 0
лезть пролезть залезть прыгнуть~
if !(%arg.contains(вверх)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Подняв решетку вы пролезли из яму.
wechoaround %actor% %actor.name% полез%actor.q% вверх из ямы.
wait 1s
wsend %actor% .- Вы вылезли из ямы.
wteleport %actor.name% 33893
wechoaround %actor% Кто-то пролез сюда из ямы.
~
#33819
в бою с воеводой~
0 l 100
~
if %actor.hitp% < 99
  msend       %actor% _Вы обессилели и не заметили как набежавшая стража схватила Вас!
  mechoaround %actor% _Набежавшая стража схватила %actor.vname% и утащила невесть куда.
  msend       %actor% _Они связали Вас, оглушили и поволокли куда то.
  msend       %actor% _Вы потеряли сознание и не заметили как очутились невесть где.
  mteleport %actor% 33897
end
~
#33820
в бою с ханом~
0 l 100
~
if %actor.hitp% < 59
  msend       %actor% Вы потеряли сознание от боли и не заметили как набежавшая стража схватила Вас!
  msend       %actor% Они связали Вас, оглушили и поволокли куда то.
  mechoaround %actor% _Набажавшая татарская стража схватила %actor.vname% и утащила невесть куда.
  msend       %actor% Последнее что вы увидели - это приближающийся к вам огромный окровавленный кол.
  msend       %actor% Вас посадили на кол!
  msend       %actor% Боль пронзила все ваше тело и вы погибли в страшных муках.
  %actor.wait(2)%
  mteleport %actor.name% 33893
  wait 1s
  eval dam %actor.hitp%+15
  mdamage %actor% %dam%
end
~
#33821
помер Хан~
0 f 100
~
if (%world.curobjs(33820)% < 2)
  mload obj 33820
end
if (%world.curobjs(33812)% < 2) && (%random.5% == 1)
  mload obj 33812
end
if (%world.curobjs(212)% < 20) && (%random.4% == 1)
  mload obj 212
end
~
#33822
лоадим священника~
2 z 100
~
wait 1
wecho  _Внезапно произошло чудо...Вместо пса здесь появился добрый Священник.
wload mob 33830
wait 3s
wecho  _"Прекрасно. Я знал что ты справишься" - сказал священник..
calcuid swiachi 33830 mob
attach 33812 %swiachi.id%
exec 33812 %swiachi.id%
calcuid roompios 33893 room
detach 33822 %roompios.id%
~
#33823
в бою с горбуном~
0 l 100
~
if %actor.hitp% < 59
  say _Ладно, хватит нам маяться.
  say _Слабоват ты еще. Поживи с мое, повоюй.
  say _До встречи, %actor.name%!
  mecho _Горбун с улыбкой убрал меч и ушел.
  mpurge %self.name%
end
~
#33824
хан орет~
0 l 100
~
wait 1
if %self.hitp% <80
  mshou  _У! Дети щакалоф и короф! За мой смирть отомстят!
  calcuid chanor 33820 mob
  detach 33824 %chanor.id%
end
~
#33830
Умер воевода - лоад сетшмутки~
0 f 100
~
if %world.curobjs(3306)% < 1  
  if  %random.100% < 3
    mload obj 3306
  end
end
~
$~
