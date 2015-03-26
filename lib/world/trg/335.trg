#33500
лезть по корням~
2 c 0
пролезть лезть~
if !(%arg.contains(вверх)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Хватаясь за корни, вы полезли вверх.
wechoaround %actor% %actor.name% полез%actor.q% вверх, хватаясь за корни.
wait 1s
wsend %actor% .- Вы оказались на узкой площадке.
wteleport %actor.name% 33526
wechoaround %actor% Кто-то пролез сюда снизу.
~
#33501
спрыгнуть вниз~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Лихо!
wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
wait 1s
wsend %actor% .- Вы на дне оврага.
wteleport %actor.name% 33515
wat 33515 wechoaround %actor% Кто-то спрыгнул сюда сверху.
~
#33502
спрыгнуть вниз~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Лихо!
wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
wait 1s
wsend %actor% .- Вы на дне оврага.
wteleport %actor.name% 33520
wat 33520 wechoaround %actor% Кто-то спрыгнул сверху.
~
#33503
спрыгнуть вниз~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Лихо!
wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
wait 1s
wsend %actor% .- Вы на дне оврага.
wteleport %actor% 33522
wat 33522 wechoaround %actor% Кто-то спрыгнул сверху.
~
#33504
спрыгнуть вниз~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Придерживая одежду, вы спрыгнули вниз... Лихо!
wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз, закрыв глаза.
wait 1s
wsend %actor% .- Вы на дне оврага.
wteleport %actor.name% 33523
wat 33523 wechoaround %actor% Кто-то спрыгнул сверху.
~
#33505
перепрыгнуть бревно~
2 c 0
прыгнуть перепрыгнуть перескочить~
if !(%arg.contains(бревно)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Разогнавшись, вы прыгнули через бревно!
wechoaround %actor% Разогнавшись, %actor.name% прыгнул%actor.g% через бревно.
wait 1s
wsend %actor% .- Вы по ту сторону бревна.
wteleport %actor% 33524
wat 33524 wechoaround %actor% Кто-то перепрыгнул бревно и оказался здесь.
~
#33506
пролезть корни~
2 c 0
пролезть лезть~
if !(%arg.contains(корни)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Вы легли на живот и полезли под корни.
wechoaround %actor% %actor.name% полез%actor.q% под корни.
wait 1s
wsend %actor% .- Вы успешно их преодолели.
wteleport %actor% 33511
wat 33511 wechoaround %actor% Кто-то пролез под корнями и оказался здесь.
~
#33507
перепрыгнуть бревно~
2 c 0
прыгнуть перепрыгнуть перескочить~
if !(%arg.contains(бревно)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Разогнавшись, вы прыгнули через бревно!
wechoaround %actor% Разогнавшись, %actor.name% прыгнул%actor.g% через бревно.
wait 1s
wsend %actor% .- Вы по ту сторону бревна.
wteleport %actor% 33523
wechoaround %actor% Кто-то перепрыгнул бревно и оказался здесь.
~
#33508
пролезть корни~
2 c 0
пролезть лезть~
if !(%arg.contains(корни)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Вы легли на живот и полезли под корни.
wechoaround %actor% %actor.name% полез%actor.q% под корни.
wait 1s
wsend %actor% .- Вы успешно их преодолели.
wteleport %actor% 33510
wat 33510 wechoaround %actor% Кто-то пролез под корнями и оказался здесь.
~
#33509
лезть нора~
2 c 0
пролезть лезть~
if !(%arg.contains(нора)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Вы попытались пролезть в нору.
wechoaround %actor% %actor.name% полез%actor.q% в нору.
wait 1s
wsend %actor% .- Вы увидели свет в норе и выбрались наружу.
wteleport %actor% 33586
wat 33586 wechoaround %actor% Кто-то пролез сюда.
~
#33510
лезть нора~
2 c 0
пролезть лезть~
if !(%arg.contains(нора)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Вы попытались пролезть в нору.
wechoaround %actor% %actor.name% полез%actor.q% в нору.
wait 1s
wsend %actor% .- Вы увидели свет в норе и выбрались наружу.
wteleport %actor% 33547
wat 33547  wechoaround %actor% Кто-то пролез сюда.
~
#33511
ломиться стена~
2 c 0
ломиться проломиться~
if !(%arg.contains(стена)%) 
  wsend       %actor% Куда это Вы хотите проломиться?
  return 0
  halt
end
wsend       %actor% Вы разогнались и прыгнули в стену... С головой все в порядке?
wechoaround %actor% %actor.name% начал%actor.g% прыгать на стены. К чему бы это? Внезапно он%actor.g% исчез%actor.q% за стеной.
wait 1s
wsend %actor% .- Вы проломились в стену и очутились в овраге.
wteleport %actor% 33573
wat 33573 wechoaround %actor% Кто-то ввалился сюда с криком.
~
#33512
ломиться стенка~
2 c 0
ломиться проломиться~
if !(%arg.contains(стенка)%) 
  wsend       %actor% Куда это Вы хотите проломиться?
  return 0
  halt
end
wsend       %actor% Вы разогнались и прыгнули в стенку... С головой все в порядке?
wechoaround %actor% %actor.name% начал%actor.g% прыгать на стенку. К чему бы это? Внезапно он%actor.g% исчез%actor.q% куда то.
wait 1s
wsend %actor% .- Вы проломились в стенку оврага и очутились в пещере.
wteleport %actor% 33587
wat 33587 wechoaround %actor% Кто-то ввалился сюда с криком.
~
#33513
дамаджнули кукушку~
0 p 100
~
wait 1
msend %damager%  Кукушка явно недовольна Вашей грубостью.
mechoaround %damager%   Кукушка явно недовольна грубостью %damager.rname%.
wait 1
mecho _Кукушка взмахнула крыльями и улетела.
calcuid kukuchka 33515 mob
mpurge %kukuchka%
~
#33514
дамаджнули кукушку 2~
0 p 100
~
wait 1
msend %damager%  Кукушка явно недовольна Вашей грубостью.
mechoaround %damager%   Кукушка явно недовольна грубостью %damager.rname%.
wait 1
msend %damager%    Кукушка взмахнула крыльями, обкидала Вас чем-то жидким и улетела.
mechoaround %damager%    Кукушка взмахнула крыльями, обкидала %damager.rname% чем-то жидким и улетела.
calcuid kukuchka 33524 mob
mpurge %kukuchka%
~
#33515
кукушка~
0 q 100
~
wait 3s
say Ку ку.
~
#33516
яйцо колется~
2 c 0
обогреть согреть высидеть сидеть греть~
if !(%arg.contains(яйцо)%) 
  return 0
  halt
end
wsend       %actor% Вы присели на яйцо верхом, и попытались его согреть.
wechoaround %actor% %actor.name% сел%actor.g% на яйцо и почувствовал%actor.g% себя орлицей.
wait 1s
wecho _"Тук - тук",- послышалось из яйца.
wait 2s
wecho _"Тук - тук",- послышалось из яйца.
wait 2s
wecho _Внезапно яйцо раскололось и появился на свет маленький мокрый птенец.
wload mob 33504
calcuid aizzoi 33510 obj
wpurge %aizzoi%
calcuid roomaiz 33554 room
detach 33516 %roomaiz.id%
~
#33517
прыгнуть в овраг~
2 c 0
прыгнуть сигануть спрыгнуть~
if (%arg.contains(овраг)%)
  switch %random.3%
    case 1
      wsend       %actor% Вы разогнались и прыгнули в овраг... 
      wechoaround %actor% %actor.name% прыгнул%actor.g% с обрыва вниз!
      wait 1s
      wsend %actor% .- Вы оказались на дне оврага.
      wteleport %actor% 33504
      wat 33504 wechoaround %actor% Кто-то спрыгнул сюда.
    break
    case 2
      wsend       %actor% Вы разогнались и прыгнули в овраг... 
      wechoaround %actor% %actor.name% прыгнул%actor.g% с обрыва вниз!
      wait 1s
      wsend %actor% .- Вы оказались на дне оврага.
      wteleport %actor% 33516
      wat 33516 wechoaround %actor% Кто-то спрыгнул сюда.
      calcuid roomaiz 33554 room
      attach 33516 %roomaiz.id%
    break
    case 3
      wsend       %actor% Вы разогнались и прыгнули в овраг... 
      wechoaround %actor% %actor.name% прыгнул%actor.g% с обрыва вниз!
      wait 1s
      wsend %actor% .- Вы оказались на дне оврага.
      wteleport %actor% 33556
      wat 33556 wechoaround %actor% Кто-то спрыгнул сюда.
      calcuid mogila 33572 room
      attach 33537 %mogila.id%
    break
    default
    break
  done
end
~
#33518
кормиш первого птенца~
0 c 0
кормить подкормить поить напоить насытить накормить~
if !(%arg.contains(птенец)%) 
  return 0
  halt
end
msend       %actor% Вы взяли с пола маленького червячка и кинули в клюв птенца.
mechoaround %actor% %actor.name% подобрал%actor.g% с пола червячка и кинул%actor.g% его в клюв птенца.
wait 1s
mecho _Птенец насытился и выглядит очень довольным.
wait 2s
mecho _Внезапно он резко увеличился в размере. Подрос наверное...
mload mob 33505
calcuid ptenezi 33504 mob
mpurge %ptenezi%
~
#33519
кормиш второго птенца~
0 c 0
кормить подкормить поить напоить насытить накормить~
if !(%arg.contains(птенец)%) 
  return 0
  halt
end
msend       %actor% Вы взяли с пола маленького червячка и кинули в клюв птенца. Скоро все червячки кончатся!
mechoaround %actor% %actor.name% подобрал%actor.g% с пола червячка и кинул%actor.g% его в клюв птенца.
wait 1s
mecho _Птенец насытился и выглядит еще более  довольным.
wait 2s
mecho _Он увеличился в размере!!! Растет не по дням а по часам.
mload mob 33506
calcuid ptenezi 33505 mob
mpurge %ptenezi%
~
#33520
трениш первого орленка~
0 c 0
тренировать учить натренировать научить~
if !(%arg.contains(орленок)%) 
  return 0
  halt
end
msend       %actor% Вы показали всем несколько боевых приемов. Орленку было очень интересно.
mechoaround %actor% %actor.name% начал%actor.g% прыгать и лягаться. Тоже мне, учитель боя!
wait 1s
mecho _Орленок задумался. По видимому, он усваивает урок.
wait 2s
mecho _У орленка резко вырос клюв и налились мышцами крылья.
mload mob 33507
calcuid pteneziii 33507 mob
attach 33521 %pteneziii.id%
exec     33521 %pteneziii.id%
calcuid ptenezi 33506 mob
mpurge %ptenezi%
~
#33521
конец игры с орленком~
0 z 100
~
wait 3s
mecho _Кажется, птенец начал подозревать, что вы не его родня. Он начал огладываться.
mecho _Осматривать свои крылья и сравнивать их с вашими руками. Несоответствие однако!
wait 1s
mecho _Внезапно к гнезду прилетел орел.
mload mob 33509
wait 1s
mecho _Он нахмурил брови и с недоумением смотрит на ваше близкое общение с его дитятей.
wait 1s
mecho _Орленок понял наконец, кто его родня, и кинулся в объятия орлу.
wait 1s
mecho _"Они не обижали тебя?", - спросил орел у сына.
wait 1s
mecho _"Обижали - обижали! Кормили всякой гадостью и учили всяким бякам!" - сказал орленок.
wait 1s
mecho _"У-ууу,  я вам щас покажу!" - сказал орленок и кинулся на Вас!
wait 1
mkill %actor%
calcuid helporel 33509 mob
attach 33536 %helporel.id%
exec     33536 %helporel.id%
~
#33522
шурудить в кустах малины~
2 c 0
шурудить тревожить потревожить пошурудить~
if !(%arg.contains(кусты)%)  || (%world.curmobs(33503)%>0)
  return 0
  halt
end
wsend %actor% Вы начали шурудить в кустах малины, пытаясь спугнуть кого-нибудь.
wechoaround %actor% %actor.name% пошурудил%actor.g% в кустах малины.
wait 1
wecho _Внезапно из под кустов выскочил перепуганный заяц и стал недоуменно оглядываться.
wload mob 33503
detach 33522 %self.id%
~
#33523
ломать кусты шиповника~
2 c 0
раздвинуть ломать двинуть двигать~
if !(%arg.contains(кусты)%) || (%world.curmobs(33518)%>0)
  return 0
  halt
end
wsend       %actor% Вы начали варварски ломать кусты шиповника.
wechoaround %actor% %actor.name% начал%actor.g% ломать кусты шиповника.
wait 1
wload mob 33518
wecho _Но кусты явно недовольны вашими действиями, и решили атаковать Вас.
detach 33523 %self.id%
~
#33524
ломать кусты малины~
2 c 0
раздвинуть ломать двинуть двигать~
if !(%arg.contains(кусты)%) || (%world.curmobs(33517)%>0)
  return 0
  halt
end
wsend       %actor% Вы начали варварски ломать кусты малины.
wechoaround %actor% %actor.name% начал%actor.g% ломать кусты малины.
wait 1
wload mob 33517
wecho _Но малина возмущена вашими действиями, и решили атаковать Вас.
detach 33524 %self.id%
~
#33525
вошел днем к охотнику~
0 q 100
~
wait 2s
mecho _Охотник взглянул на Вас и продолжил делать свои дела.
~
#33526
помер заяц~
0 f 100
~
if (%world.curobjs(33500)% < 30) && (%random.2% == 1)
  mload obj 33500
end
~
#33527
помер заяц белый~
0 f 100
~
if (%world.curobjs(33500)% < 30) && (%random.2% == 1)
  mload obj 33500
end
if (%world.curobjs(33501)% < 30) && (%random.2% == 1)
  mload obj 33501
end
~
#33528
помер заяц серый~
0 f 100
~
if (%world.curobjs(33500)% < 30) && (%random.2% == 1)
  mload obj 33500
end
if (%world.curobjs(33502)% < 30) && (%random.2% == 1)
  mload obj 33502
end
~
#33529
помер орлица~
0 f 100
~
if %random.4%== 1 && %world.curobjs(33503)% < 2
  mload obj 33503
end
~
#33530
помер лис~
0 f 100
~
if (%world.curobjs(33504)% < 20) && (%random.2% == 1)
  mload obj 33504
end
~
#33531
помер малина~
0 f 100
~
if (%world.curobjs(33505)% < 20) && (%random.2% == 1)
  mload obj 33505
  mload obj 33505
  mload obj 33505
end
~
#33532
помер шиповник~
0 f 100
~
if (%world.curobjs(33506)% < 20) && (%random.2% == 1)
  mload obj 33506
  mload obj 33506
  mload obj 33506
end
~
#33533
помер кабанчик~
0 f 100
~
if (%world.curobjs(33508)% < 15) && (%random.2% == 1)
  mecho Голова кабана отвалилась от тела при падении на землю.
  mload obj 33508
end
~
#33534
помер призрак~
0 f 100
~
if (%world.curobjs(33509)% < 1) && (%random.4% == 1)
  mload obj 33509
end
~
#33535
дал шкурки охотнику~
0 j 100
~
if %object.vnum% == 33501
  wait 1s
  say Посмотрим, что ты принес...
  wait 2s
  mecho Охотник начал ловко отделывать шкурку.
  calcuid varriro 33501 obj
  mpurge %varriro.name%
  mload obj 33511
  wait 2s
  дат шкурк %actor.name%
end
if %object.vnum% == 33502
  wait 1s
  say Посмотрим, что ты принес...
  wait 2s
  mecho Охотник начал ловко отделывать шкурку.
  calcuid varri 33502 obj
  mpurge %varri.name%
  mload obj 33512
  wait 2s
  дат шкурк %actor.name%
end
if %object.vnum% == 33504
  wait 1s
  say Посмотрим, что ты принес...
  wait 2s
  mecho Охотник начал ловко отделывать шкурку.
  calcuid varro 33504 obj
  mpurge %varro.name%
  mload obj 33513
  wait 2s
  дат лис %actor.name%
end
брос все
~
#33536
орел помогает орленку~
0 z 100
~
wait 1s
mkill %actor%
~
#33537
копаем могилу~
2 c 0
копать раскопать ломать~
if !(%arg.contains(могила)%) 
  return 0
  halt
end
wsend       %actor% Засучив рукава, вы начали раскапывать могилу.
wechoaround %actor% %actor.name%, стал%actor.g% варварски рушить могилу. Кошмар!
wecho Из могилы быстро просочился наружу черный дым и превратился в призрака.
wload mob 33519
detach 33537 %self.id%
~
#33538
помер злой охотник~
0 f 100
~
*if (%world.curobjs(616)% < 40) && (%random.4% == 1)
*   mload obj 616
*end
~
#33539
Репоп зоны "большие яры"~
2 f 100
~
calcuid coffin 33572 room
detach 33537 %coffin.id%
attach 33537 %coffin.id%
calcuid eg 33554 room
detach 33516 %eg.id%
attach 33516 %eg.id%
calcuid mogila 33572 room
detach 33537 %mogila.id%
attach 33537 %mogila.id%
~
$~
