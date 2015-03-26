#30800
леший приветствует~
0 r 100
~
mecho _Пучеглазый Леший радостно хлопнул в ладоши.
wait 1s
г Этот старый, трухлявый пень послал тебя,
г отнять у меня сбежавших от него белок?
wait 1s
mecho _В глазах Лешего сверкнули две молнии.
wait 1s
г Хотя постой, мы можем все решить мирно...
wait 2s
г Вот уже тысячу лет охраняю я Дуб и Березу на этой поляне.
г Да вот беда, поселились на деревьях дятлы огромные,
г спасенья от них нет, долбят и долбят деревья...
wait 1s
г Если бы не эти белки, которых я приучил есть дятлов,
г  то деревьям пришел бы конец.
wait 1s
г Так что уничтожь на деревьях дятлов, а главное их мерзкого царька.
г И я так и быть отпущу с тобой белок. А теперь ступай...
~
#30801
лешему дали череп~
0 j 100
~
if %object.vnum% == 30807
  wait 1s
  говор Ты сделал это, молодец!
  wait 1s
  mecho _Пучеглазый Леший постучал пальцем по черепу и прислушался к гулу.
  wait 1s
  говор Без своего Царя дятлы скоро погибнут или уйдут искать себе нового.
  if (%world.curobjs(30808)% < 4) &&  (%random.4% == 2) 
  mecho _Пучеглазый Леший взглянул на череп в руках и произнес: "Трах-Тябидох".
  wait 1s
говор На вот, возьми, мне он ни к чему, а тебе, может быть, и пригодится.
  mload obj 30808
  дат череп %actor.name%
  end
  wait 1s
говор А теперь, как я и обещал, верну тебе белок.
  wait 1s
  mecho _Пучеглазый Леший сделал загадочный жест.
  wait 1s
  mecho _Зубастая белка забежала в берлогу.
  mecho _Зубастая белка забежала в берлогу.
  mecho _Зубастая белка забежала в берлогу.
  mload mob 30813
  mload mob 30813
  mload mob 30813
  wait 2s
  говор Ну чтож, прощайте... буду вас навещать иногда.
  wait 1s
  mecho _Пучеглазый Леший сделал загадочный жест.
  wait 1s
  calcuid belka 30813 mob
  %purge% %belka%
*  attach 30813 %belka.id%
*  exec 30813 %belka.id%
  calcuid belka 30813 mob
  %purge% %belka%
*  attach 30813 %belka.id%
*  exec 30813 %belka.id%
  calcuid belka 30813 mob
  %purge% %belka%
*  attach 30813 %belka.id%
*  exec 30813 %belka.id%
mecho  Белки стали уменьшаться, пока не превратились в маленькие комочки.
wait 1s
  mecho _Пучеглазый Леший подобрал белок и связал им хвосты.
wait 1s
  говор На вот, а теперь прощай...
  mload obj 30809
  дат связка %actor.name%
  mecho _Пучеглазый Леший сделал загадочный жест.
  wait 1s
  %purge% %object%
  calcuid berlom 30812 room
  attach 30805 %berlom.id%
  exec 30805 %berlom.id%
  calcuid leshiy 30701 mob
  detach 30723 %leshiy.id%
  detach 30801 %self.id% 
end
~
#30802
убили Царя дятлов~
0 f 100
~
calcuid leshiy2 30800 mob
detach 30800 %leshiy2.id%
mecho _Голова Царя дятлов обуглилась, превратившись в желтый череп.
mload obj 30807
~
#30803
репоп зоны~
2 f 100
~
calcuid leshiy2 30800 mob
detach 30800 %leshiy2.id%
detach 30801 %leshiy2.id%
attach 30800 %leshiy2.id%
attach 30801 %leshiy2.id%
calcuid berlom 30812 room
detach 30805 %berlom.id%  
if %exist.mob(30813)%
calcuid belka 30813 mob
detach 30813 %belka.id%
end
~
#30804
зашли в клетку к Царю Дятлов~
0 q 100
~
mecho _Царь Дятлов злобно посмотрел в Вашу сторону.
говор Ти пришел убить миня, ну так умри типерь сам!
mkill %actor%
~
#30805
создаем пенту~
2 z 100
~
wportal 30704 2
~
#30806
убили пестрого дятла~
0 f 100
~
if (%world.curobjs(30804)% < 5) &&  (%random.3% == 1)
mload obj 30804
end
~
#30807
убили плотоядного дятла~
0 f 100
~
if (%world.curobjs(30802)% < 10) &&  (%random.3% == 1)
mload obj 30802
end
~
#30808
убили златоклювого дятла~
0 f 100
~
if (%world.curobjs(30800)% < 5) &&  (%random.3% == 1)
mload obj 30800
end
~
#30809
убили белокрылого дятла~
0 f 100
~
if (%world.curobjs(30803)% < 5) &&  (%random.3% == 1)
mload obj 30803
end
~
#30810
убили бескрылого дятла~
0 f 100
~
if (%world.curobjs(30801)% < 20) &&  (%random.3% == 1)
mload obj 30801
end
~
#30811
убили старого дятла~
0 f 100
~
if (%world.curobjs(30805)% < 4) &&  (%random.3% == 1)
mload obj 30805
end
~
#30812
убили кукуху~
0 f 100
~
if (%world.curobjs(30806)% < 3) &&  (%random.3% == 1)
mload obj 30806
end
~
#30813
убираем белок из берлоги~
0 z 100
~
mpurge self
~
#30814
залазим в гнездо~
2 c 0
лезть залезть~
if !(%arg.contains(гнездо)%) 
   wsend       %actor% Куда это Вы хотите пролезть???
   return 0
   halt
end
  wsend       %actor% Хватаясь за ветки дуба, вы полезли в птичье гнездо.
  wechoaround %actor% _%actor.name% полез%actor.q% в птичье гнездо, хватаясь за ветки дуба.
  wait 1s
  wsend %actor% - Вы оказались на дне большого гнезда.
  wteleport %actor.name% 30891
  wat 30891 wechoaround %actor% Кто-то залез в гнездо.
~
#30815
вылазим из гнезда~
2 c 0
лезть вылезть~
if !(%arg.contains(назад)%) 
   wsend       %actor% Куда это Вы хотите пролезть???
   return 0
   halt
end
  wsend       %actor% _Осторожно хватаясь за ветки дуба, вы полезли назад.
  wechoaround %actor% _%actor.name% полез%actor.q% назад, осторожно хватаясь за ветки дуба.
  wait 1s
  wsend %actor% - Вы оказались на огромной дубовой ветке.
  wteleport %actor.name% 30834
  wat 30834 wechoaround %actor% Кто-то вылез из птичьего гнезда.
~
#30816
одеваем глаз дятла~
1 j 100
~
osend       %actor% _Глаз дятла на Вашем пальце загадочно Вам подмигнул.
oechoaround %actor% _Глаз дятла на пальце %actor.rname% загадочно Вам подмигнул.
~
#30817
Есть игроки в 307 зоне?~
2 b 100
~
eval test 1
global test
calcuid resetroom 30809 room
remote test %resetroom.id% 
rdelete test %self.id%
detach 30817 %self.id%
~
$~
