#38500
ломаем амфору 38511~
1 c 4
разбить~
if !%arg.contains(амфора)%
  osend       %actor% Что Вы хотите разломать???
  halt
end
osend       %actor% _Вы мощным ударом разбили амфору.
oechoaround %actor%  %actor.name% мощным ударом разбил%actor.g% амфору.
wait 1s
if !%world.curmobs(38511)%
  oecho _Комнату заполнил светлый дымок и тут же рассеялся.
  oload mob 38521
else
  oecho _Комнату заполнил темный дымок и тут же рассеялся.
  oload mob 38509
end
opurge  %self%
~
#38501
ломаем амфору 38508~
1 c 4
разбить~
if !%arg.contains(амфора)%
  osend       %actor% Что Вы хотите разломать???
  halt
end
osend       %actor% _Вы мощным ударом разбили амфору.
oechoaround %actor% _%actor.name% мощным ударом разбил%actor.g% амфору.
wait 1s
if !%world.curmobs(38511)%
  oecho _Комнату заполнил светлый дымок и тут же рассеялся.
  oload mob 38522
else
  oecho _Комнату заполнил темный дымок и тут же рассеялся.
  oload mob 38510
end
opurge  %self%
~
#38502
очистка  зоны после смерти  чернока и быстрый репоп~
2 z 100
~
set fmob 38502 
while %fmob% < 38535
  set i %world.curmobs(%fmob%)%
  while %i% > 0  
    calcuid pm %fmob% mob
    exec 38517 %pm.id%
    wait 1s 
    eval i %i%-1
  done
  eval fmob %fmob%+1    
done    
wait 3t
if %world.curmobs(38511)%
  halt
end
wait 5t
if !%world.curmobs(38511)%
  %world.zreset(385)%
end
~
#38503
подходят к драконам ~
0 q 100
~
if %actor.class% == 8
  wait 1s
  привет %actor.name%
  if %actor.sex% == 2
    wait 1s
    say Проходи с миром, дочь Тьмы.
  else
    wait 1s
    say Рад видеть тебя, Хозяин.
  end
end
~
#38504
подходят к безголовому~
0 q 100
~
if !(%actor.class% == 8)
  wait 1s
  встать
  msend %actor% _Скелет почуял исходящее от Вас добро и это заставило его задрожать от гнева.
  wechoaround %actor% _Скелет почуял исходящую от %actor.rname% добрую силу!
  mkill %actor%
else
  wait 1s
  поклон %actor%.
end
end
~
#38505
лоад мобов бывшей 386-й зоны~
2 bz 100
~
if !%world.curmobs(38511)%
  wait 3t
  halt
end
set i 0
set j %random.7%
switch %random.8%  
  case 1
    set vnum 38527
  break
  case 2
    set vnum 38528
  break         
  case 3
    set vnum 38529
  break
  case 4
    set vnum 38534
  break 
  default         
    set vnum 38534
    set i 10
  break
done       
foreach mogila 38512 38515 38523 38527 38531 38536 38553   
  if %world.curmobs(%vnum%)% > 2
  break
end
eval i  %i%+1
if %i% == %j%
  wload mob %vnum%
  wdoor 38597 up room %mogila%
  wforce моб_385 вст
  wforce моб_385 вверх  
  wait 1s
  wdoor 38597 up purge
end
done
wait 3s
switch %random.4%  
  case 1
    set j %random.4%
    set spis 38524 38525 38537 38552
    set vnum 38533
  break         
  default                  
    set j %random.5%
    set spis 38585 38565 38579 38588 38592
    set vnum 38530
  break
done  
if %world.curmobs(%vnum%)% > %j%
  halt
end
set i 0     
foreach mogila %spis%
  eval i  %i%+1
  if %i% == %j%
    wload mob %vnum%
    wdoor 38597 down room %mogila%
    wforce моб_385_2 вст
    wforce моб_385_2 вниз
    wait 1s
    wdoor 38597 down purge
  end
done
~
#38506
подходят к безголовому!~
0 q 100
~
if !(%actor.class% == 8)
  встать
  if (%world.curobjs(38501)% < 20) && (%random.2% == 1)  
    mload obj 38501
    mecho _Безголовый скелет выломал у себя кость.
    вооруж кость
    метнуть %actor.name%
  end
  mkill %actor%
else
  wait 1s
  поклон %actor%.
end
end
~
#38507
подходят к злому мобу ~
0 q 100
~
if !(%actor.class% == 8)
  встать
  mkill %actor%
else
  wait 1s
  улыб %actor%.
end
end
~
#38508
чернок приходит в склеп~
0 s 100
~
if %self.realroom% == 38562
  if %world.curobjs(38511)%
    wait 1s
    mecho _Чернокнижник внимательно осмотрел амфору.
    mecho _Убедившись в ее сохранности он улыбнулся и съел кусок человечьего мяса.
  else
    wait 1s
    mecho _Чернокнижник старательно осмотрел склеп,
    mecho _и, не найдя амфоры заревел от злости!
    mecho _Стал рвать на себе волосы и пинать себя.
    wait 2s
    mecho _Потом чернокнижник одумался и начал искать разорителя склепа.
    if %random.pc%
      set target %random.pc%
      mkill target
    end
  end
end
~
#38510
трахаем паутину~
1 c 4
дернуть~
if !(%arg.contains(паутина)%)
  osend       %actor% Что Вы хотите потревожить???
  return 0
  halt
end
osend       %actor% _Вы взяли одну из нитей паутины и начали ее дергать.
oechoaround %actor%  %actor.name% взял%actor.g% одну из нитей паутины и начал%actor.g% ее дергать.
wait 1s
oecho _Вскоре появился большой разноцветный паук, и стал искать тупым взглядом свой завтрак.
oload mob 38508
wait 1
opurge %self%
~
#38511
ведьма приходит в склеп~
0 s 100
~
if %self.realroom% == 38547
  if %world.curobjs(38508)% 
    wait 1s
    wecho _Ведьма осторожно осмотрела амфору и подсыпала туда еще что-то.
  else
    wait 1s
    wecho _Ведьма не нашла амфору, задумалась, а потом облегченно вздохнула.
  end
end
~
#38512
помер безголовый~
0 f 100
~
if (%world.curobjs(38500)% < 10) && (%random.4% == 1)
  mecho _Безголовый труп бросил пусту голову на землю и рассыпался в прах.
  mload obj 38500
end
~
#38513
помер ведьма~
0 f 100
~
if (%world.curobjs(38513)% < 15) && (%random.2% == 1)
  mload obj 38513
end
~
#38514
свежуем паука~
1 c 4
освежевать~
if !(%arg.contains(паук)%) 
  osend       %actor% Что Вы хотите освежевать ?!!
  return 0
  halt
end
osend       %actor% _Вы попытались освежевать труп паука , но мяса не нашли, а увидели лишь ключик.
oechoaround %actor%  %actor.name% немного освежевал%actor.g% труп паука но мяса не нашел.
oecho _Маленький ключик звонко упал на каменный пол.
oload obj 38515
wait 1
opurge %self%
~
#38515
помер паук~
0 f 100
~
if !%world.curobjs(559)% && %random.10%==1
  mload obj 559
end     
mload obj 38528
~
#38516
помер главный~
0 f 100
~
while %exist.mob(38532)%
  calcuid badwind 38532 mob
  %purge% %badwind%
done
mload obj 38516
mecho _Злые силы, покрывавшие округу, рассеялись. 
calcuid komn 38595 room
exec 38502 %komn.id%  
~
#38517
пуржим себя~
0 z 100
~
wait 1
mpurge %self%
~
#38518
вызов  проклятых ветров~
0 k 100
~
wait 1s   
if %random.2% == 1 && %world.curmobs(38532)% < 3
  mecho _Проклятый ветер влетел в окно!
  mload mob 38532                                 
end
~
#38520
лоад мил человека~
0 n 100
~
wait 1s
пла
say Спасибо за освобождение! Но сердце мое не спокойно....
wait 1s
say Ой неспокойно... Жена моя, милая лебедушка... Где ты?
wait 1
say Ни есть ни спать без нее не могу, радость жизни моей, найти хочу...
wait 1
say Похитили ее вместе со мной, и обещали заточить в застенки каменные.
wait 1
say Где то она, моя ладушка, горюет сейчас, где печалится...
wait 1
say Уж не Кащей ли над ней потешается, уж не Соловей ли развлекается..?
пла
calcuid chelow 38521 mob
detach 38520 %chelow.id%
~
#38521
говорим следуй мил человеку~
0 d 0
следуй гоу пошли провожу~
wait 1s
say Проводи же меня...
след %actor.name%
detach 38521 %self.id%
~
#38522
целуем мыш~
0 c 0
целовать ласкать обнять трахать тронуть~
if !(%arg.contains(мышка)%) 
  msend %actor% Кого вы хотите тронуть???
  return 0
  halt
end
msend %actor% Вы прикоснулись к мышке.
mechoaround %actor% %actor.name% нагло тронул%actor.g% беззащитную мышку.
wait 1s
mecho _Внезапно мышь резко увеличилась и превратилась в высокую красавицу..
mload mob 38524
mpurge %self%
~
#38523
говориш челу что надо делать~
0 d 0
целуй трогай обними ласкай поцелуй ~
if ((%self.realroom% != 38547) || !%world.curmobs(38522)%)
  wait 1s
  mecho Мил человек задумался.
  say Что то я тебя не пойму...
  return 0
  halt
end
wait 1s
mecho _Мил человек задумался, взглянул на мышку.
mecho _На его лице проскользнула тень сомнения...
wait 2s
say Ну, была - не была!
wait 1s
mecho  _Он нагнулся к мышке, погладил ее и легонько прикоснулся губами к ее носику.
mecho _Внезазапно мышка резко увеличилась и обернулась высокой красавицей!
wait 1s
рад
mload mob 38523
calcuid miska 38522 mob
mpurge %miska%
mecho _Влюбленные обнялись, поцеловались троекратно (во все места)... и собрались уже уходить..
mecho _Но тут муж вспомнил о своем спасителе-избавителе!
mecho _Поклонился троекратно, радостно попрощался (им надо так много рассказать друг другу)..
mecho _и чуть было не был таков, вместе со своей молодой женой...
mecho _Он вспомнил что у него завалялся где-то священный напиток русичей.
mecho _Якобы он поднимает из мертвых кого угодно...
wait 1s
say Прими же его, как награду за труды!!
mload obj 38502
дать напит %actor.name%
calcuid a2mfor 38523 mob
mpurge %a2mfor%
if (%world.curobjs(566)%==0) && (%random.10%==1)
  mload obj 566
  say Ну и вот еще что возьми, совсем забыл, может пригодится...
  дать книг %actor.name%
elseif (%world.curobjs(540)%==0) && (%random.9%==1)
  mload obj 540
  say Ну и вот еще что возьми, совсем забыл, может пригодится...
  дать книг %actor.name%
elseif (%world.curobjs(573)%==0) && (%random.8%==1)
  mload obj 573
  say Ну и вот еще что возьми, совсем забыл, может пригодится...
  дать книг %actor.name%
end
пока
mpurge  %self%
~
#38524
помер высокая красавица~
0 f 100
~
if ((%world.curobjs(209)% < 50) && (%random.1000% <= 250))
  mload obj 209
end
~
#38527
открываем стол~
1 p 100
~
wait 1
oecho _Из стола выскочил маленький зеленый демон.
oload mob 38525
oecho _Маленький зеленый демон мерзко захихикал.
detach 38527 %self.id% 
~
#38528
вооружаемся головой~
1 j 100
~
osend       %actor% _Вы подбросили пусту голову и ловко поймали ее пальцами за глазницы.
oechoaround %actor% _%actor.name% подбросил%actor.g% пусту голову и ловко поймал%actor.g% ее за пустые глазницы.
~
#38529
вооружаемся палкой~
1 j 100
~
if %world.curobjs(38517)% < 4
  if (%actor.class% == 8) ||  (%actor.class% == 0) ||  (%actor.class% == 1) ||  (%actor.class% == 6) ||  (%actor.class% == 7)
    osend  %actor% _Кривая палка засияла всеми цветами радуги!
    osend  %actor%  Внезапно она стала удлиняться, выпрямляться и..
    osend  %actor% _Превратилась в волшебный сверкающий посох!
    oload obj 38517
    oforce %actor% взя волш
    wait 1
    opurge %self%
  end
end
~
#38530
помер чернокнижник молодой~
0 f 100
~
mecho _Дух чернокнижника восстал из мертвых и будет вечно преследовать Вас!
mload mob 38526
~
#38535
помер скелет со стабером~
0 f 100
~
if (%world.curobjs(38518)% < 15) && (%random.2% == 1)
  mload obj 38518
end
~
#38537
помер бесовка с ребрами~
0 f 100
~
if (%world.curobjs(38519)% < 11) && (%random.2% == 1)
  mload obj 38519
end
~
#38538
помер упырь с кафтаном~
0 f 100
~
if (%world.curobjs(38520)% < 10) && (%random.2% == 1)
  mload obj 38520
end
~
#38539
помер упыренок~
0 f 100
~
if (%world.curobjs(38521)% < 15) && (%random.2% == 1)
  mload obj 38521
end
~
#38540
помер красавица~
0 f 100
*~
if ((%world.curobjs(209)% < 50) && (%random.1000% <= 250))
  mload obj 209
end
if (%world.curobjs(38522)% < 10) && (%random.2% == 1)
  mload obj 38522
end
if (%world.curobjs(1204)% < 1) && (%random.100% <= 3)
  mload obj 1204
end
~
#38542
подходим к ползущему~
0 q 100
~
if !(%actor.class% == 8)
  встать
  kill %actor%
else
  wait 2s
  mecho Безногий скелет попытался поклониться но лишь ткнулся черепом в землю...
end
~
#38543
подходим к дракону~
0 q 100
~
if !(%actor.class% == 8)
  встать
  mecho _Костяной дракон воинственно поднял крылья!
  kill %actor%
else
  wait 1s
  mecho _Костяной дракон вежливо помахал Вам крылами.
end
~
#38544
ноги встречают тело~
0 i 100
~
if %actor.vnum% == 38528
  wait 1s
  mecho _Гнилой скелет обрадовался как ребенок при виде своих ног!
  calcuid gniloi 38528 mob
  mpurge %gniloi%
  wait 1s
  mecho _Он приставил костяшки ног к себе и довольно побрел дальше.
  mload mob 38531
  mpurge %self%
end
~
#38545
тело встречает ноги~
0 i 100
~
if %actor.vnum% == 38529
  wait 1s
  mecho _Гнилой скелет обрадовался как ребенок при виде своих ног!
  calcuid gniloi 38529 mob
  mpurge %gniloi%
  wait 1s
  mecho _Он приставил костяшки ног к себе и довольно побрел дальше.
  mload mob 38531
  mpurge %self%
end
~
#38546
тело встречает ноги~
0 q 100
~
if %actor.vnum% == 38529
  wait 1s
  mecho _Гнилой скелет опечалился при виде ног.
  say Не мои!
  плак
end
~
#38547
подходим к гнилому 2~
0 q 100
~
if !(%actor.class% == 8)
  встать
  kill %actor%
else
  wait 2s
  mecho _Гнилой скелет склонился перед вами в глубоком реверансе.
end
~
#38548
помер грязный~
0 f 100
~
if (%world.curobjs(38529)% < 15) && (%random.3% == 1)
  mload obj 38529
end
~
#38549
помер гнилой~
0 f 100
~
if (%world.gameobjs(38530)% < 15) && (%random.2% == 1)
  mload obj 38530
end
~
$~
