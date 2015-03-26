#33000
вход к умельцу (33024)~
0 q 100
~
wait 1s
say Ходят тут всякие, а потом вещи пропадают. Вот недавно какой-то ворюга стащил мои инструменты.
вздох
wait 5s
say Что бы я ни отдал за них.
~
#33001
корчмарь приветствует (33027)~
0 q 100
~
wait 1
say Привет путник! Как дела, выпей, посиди отдохни. А я пока тебя поразвлекаю местными байками, ну естественно за небольшое вознаграждение. 
wait 1
улыб %actor.name%
~
#33002
дали 100 монет корчмарю (33027)~
0 m 1
~
wait 1
if %amount% < 100 then
  say Маловато будет,  а моя помощь вам пригодится.
  halt
end
switch %myvar%
  case 1
    say - Да говорят  ворюгу приютила, живет с ним а мож вместе крадет!!! Во..
    wait 1
    ухм
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 2
    global myvar
  break
  case 2
    say - Токо ворюгу днем не найти, он по ночам ходит и очень осторожничает и прячется...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 3
    global myvar
  break
  case 3
    say - За северным островом водятся чудо рыбы, ужасные и кровожадные, туда никто не ходит а недавно они съели девочку...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 4
    global myvar
  break
  case 4
    say - плавник чудо рыбы обладает рядом чудесных свойств...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 5
    global myvar
  break
  case 5
    say - Старый рыбак то наш - золотые руки, может из плавника сделать ожерелье, что человеку никакая вода не страшна...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 6
    global myvar
  break
  case 6
    say - У смотрителя в складах, говорят, такое есть!!! Или под складами... Не помню...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 7
    global myvar
  break
  case 7
    say - Путник говорил, а может врал, что степняки надвигаются на Русь великой силой!!!
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 8
    global myvar
  break
  case 8
    say - Говорят, что у старика, живущего неподалеку, утонули все три дочери!!! А он, бедолага, все ждет их и верит, что они живы!!!
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 9
    global myvar
  break
  case 9
    say - Что, кун многовато? Прости, дел много, некогда с тобой лясы точить...да и не о чем...
    wait 1
    mecho Корчмарь отвернулся от Вас и занялся своими делами.
    set myvar 0
    global myvar
  break
  default
    say - Ведьма наша, совсем с ума сошла, на всех кидается, житья нет...
    wait 1
    mecho Корчмарь замолкнул, подмигнул и похлопал по карману.
    set myvar 1
    global myvar
  break
done
~
#33003
у купца ~
0 q 100
~
wait 1
say Сегодня ничего не продаю и не покупаю. Иди вон отсюда, бродяга!!!
wait 1s
mecho  _Купец потребовал всем удалиться.
say Иди от сюда, проходимец!!!.
msend  %actor%  Купец пригрозил пальцем, повернул Вас к себе задом и БОЛЬНО ударил ниже спины сапогом!
mechoaround %actor% %actor.name%, получив удар купчишки, с криком вылетел%actor.g% из дома!!! 
wait 1s
msend %actor% - Потирая зад, Вы стоите у купеческого дома.
mteleport %actor% 33043
calcuid zellroom 33043 room
attach 33018 %zellroom.id%
exec  33018 %zellroom.id%
detach 33018 %zellroom.id%
calcuid kupez 33013 mob
attach 33013 %kupez.id%
calcuid kupez 33013 mob
detach 33003 %kupez.id%
~
#33004
вход к смотрителю ~
0 r 100
~
wait 10
say Ты, наверное, прибыл%actor.g% меня ограбить!!!
say Не выйдет!!!.
встать
kill %actor.name%
end
~
#33005
вход к старому подслеповатому рыбаку~
0 r 100
~
wait 1s
say Здравствуй путник....
wait 1s
say Чем порадуешь бедного старика?
~
#33006
вход к несчастному старику~
0 q 100
~
wait 1
msend %actor%  Старик печально взглянул на Вас.
mechoaround %actor% Старик печально взглянул на %actor.vname%
~
#33007
дал инстр ум-цу~
0 j 100
~
wait 1
if %object.vnum% == 33019
  wait 1
  mpurge %object%
  wait 2s
  say Спасибо. Выручил%actor.g%!
  wait 2s
  if (%world.curobjs(242)% < 50) && (%random.3% == 2)
    mecho _Умелец достал руну с полки.
    mload obj 242
    wait 2s
    дат рун %actor.name%
  else
    mecho _Умелец взял крепкую сеть с пола.
    mload obj 33000
    wait 2s
    дат сеть %actor.name%
  end
end
else
say  Зачем мне это? 
eval getobject %object.name%
брос %getobject.car%.%getobject.cdr%
end
~
#33008
дал плавник старику~
0 j 100
~
wait 1
if %object.vnum% == 33017 then
  wait 1
  mpurge %object%
  wait 2s
  say Ох. Опять эти плавники... Ну ладно, сделаю тебе ожерелье, только смотри не умри от него! 
  wait 2s
  mecho Старец ловко обработал плавник..
  wait 2s
  mload obj 33018
  дат ожер .%actor.name%
end
~
#33009
вход к одинокой рыбачке~
0 g 100
~
wait 2s
mecho  Рыбачка улыбнулась Вам и предложила остаться у нее переночевать
~
#33010
даешь 10 монет девице~
0 m 10
~
if %amount% < 10 then
  wait 1
  say - За такие деньги пойди сам налови да пожарь!!!.
else
  wait 1s
  say - Принесу самую свежую.
  wait 1s
  mecho _Девица убежала куда-то.
  wait 1s
  mecho _Быстро прибежала с подносом рыбы, выбрала самую аппетитную.
  mload obj 33003
  дат рыб %actor.name%
end
~
#33011
дал локон старику~
0 j 100
~
wait 1
if ((%object.vnum% < 33309) || (%object.vnum% > 33311))
  drop all
  halt
end
set objvnum %object.vnum%
mpurge %object%
wait 1s
set i 0
switch %objvnum%
  case 33309
    if ((%world.curobjs(3381)% < 2) && (%random.10000% <= 50))
      say Прими вот это кольцо!
      mload obj 3381
      wait 10
      дать коль .%actor.name%
    else
      set i 1
    end
  break
  case 33310
    wait 1s
    if ((%world.curobjs(33307)% < 4) && (%random.1000% <= 333))
      say Спасибо!
      wait 1s
      mload obj 33307
      wait 10
      say Прими вот это кольцо!
      дать коль.мудр %actor.name%
    else
      set i 1
    end
  break
  case 33311
    wait 1s
    if ((%world.curobjs(33308)% < 4) && (%random.1000% <= 333))
      say Спасибо!
      wait 1s
      mload obj 33308
      wait 10
      say Прими вот это кольцо!
      дать коль.бог %actor.name%
    else
      set i 1
    end
  break
done
if %i%
  say Спасибо!
  %self.gold(+20000)%
  дать 20000 кун .%actor.name%
end
~
#33012
в водовороте~
2 e 100
~
wait 3s
wecho _Стихия воды кружит Вас и у Вас разболелась голова.
wait 5s
wecho _Стихия воды кружит Вас и у Вас разболелась голова.
wait 10s
wecho _Стихия воды кружит Вас и у Вас разболелась голова.
wait 15s
wecho _Стихия воды кружит Вас и у Вас разболелась голова.
~
#33013
вход к купцу 2~
0 q 100
~
wait 1
say Я же всех предупреждал чтоб не лезли ко мне сегодня!!!
встать
mkill %actor%
~
#33014
репоп тригеров~
2 f 100
~
calcuid kupez 33013 mob
detach 33013 %kupez.id%
calcuid kupez 33013 mob
attach 33003 %kupez.id%
calcuid udochkki 33004 room
attach 33023 %udochkki.id% 
calcuid udochkl 33004 room
exec 33026  %udochkl.id%
~
#33015
убитопытрыбак~
0 f 20
~
if %world.curobjs(501)% == 0
  mload obj 501
end
mload obj 33036
~
#33016
убкупжена~
0 f 20
~
switch %random.2%
  case 1
    if %world.curobjs(504)% == 0
      mload obj 504
    end
  break
  case 2
    if ((%world.curobjs(219)%<50) && (%random.5% == 1))
      mload obj 219
    end
  break
  default
  break
done
~
#33017
убподслепрыбак~
0 f 20
~
if %world.curobjs(521)% == 0
  mload obj 521
end
~
#33018
когда игрок вылетает от купца~
2 z 100
~
wechoaround %actor% Кто-то с жутким криком прилетел сюда из купеческого терема и брякнулся о землю.
~
#33019
вошли в подвал у смотрителя~
2 e 100
~
wait 2s
wecho Сторожевой пес прибежал сюда на шум.
wload mob 33022
wait 5s
wecho Сторожевой пес прибежал сюда на шум.
wload mob 33022
detach 33019 %self.id%
~
#33020
умер смотритель~
0 f 100
~
if %world.curobjs(522)% == 0
  mload obj 522
end
mload obj 33015
mload obj 33032
~
#33021
прыгнуть через разлом туда~
2 c 0
прыгнуть перепрыгнуть сигануть~
if !(%arg.contains(разлом)%) 
  wsend       %actor% Что вы хотите перепрыгнуть???
  return 0
  halt
end
wsend       %actor% Разбежавшись, вы прыгнули через разлом моста.
wechoaround %actor% Разбежавшись, %actor.name% прыгнул%actor.g% через разлом.
wait 1s
wsend %actor% .- Вы по ту сторону разлома.
wteleport %actor% 33015 horse
wechoaround %actor% Кто-то прыгнул сюда.
~
#33022
прыгнуть через разлом назад~
2 c 0
прыгнуть перепрыгнуть сигануть~
if !(%arg.contains(разлом)%) 
  wsend       %actor% Что вы хотите перепрыгнуть???
  return 0
  halt
end
wsend       %actor% Разбежавшись, вы прыгнули через разлом моста.
wechoaround %actor% Разбежавшись, %actor.name% прыгнул%actor.g% через разлом.
wait 1s
wsend %actor% .- Вы по ту сторону разлома.
wteleport %actor% 33012 horse
wechoaround %actor% Кто-то прыгнул сюда.
~
#33023
закинуть удочку~
2 c 0
закинуть забросить ловить рыбалить~
if !(%arg.contains(удочка)%) 
  wsend       %actor% Что вы хотите закинуть???
  return 0
  halt
end
wait 1
wsend       %actor% Проверив наживку и хорошенько размахнувшись, вы закинули удочку.
wechoaround %actor% Осмотрев наживку и состроив из себя опытного рыбака, %actor.name% закинул%actor.g% удочку в реку.
wpurge  fishhook330
wload obj 33033
calcuid udorchkk 33004 room
attach 33024 %udorchkk.id% 
exec  33024 %udorchkk.id% 
calcuid urdochkki 33004 room
detach 33023 %urdochkki.id% 
~
#33024
клюет~
2 z 100
~
switch %random.3%
  case 1
    wait 15s
    wecho _Клюет. Самое время подсечь.
    calcuid uqdochkki 33004 room
    attach 33025 %uqdochkki.id% 
  end
break
case 2
  wait 10s
  wecho _Клюет. Самое время подсечь.
  calcuid uwdochkkii 33004 room
  attach 33025 %uwdochkkii.id% 
end
break
case 3
  wait 5s
  wecho _Клюет. Самое время подсечь.
  calcuid uedochkkk 33004 room
  attach 33025 %uedochkkk.id% 
end
break
done
~
#33025
подсечь удочку~
2 c 0
подсечь поднять дернуть~
if !(%arg.contains(удочка)%) 
  wsend       %actor% Что вы хотите подсечь???
  return 0
  halt
end
wsend       %actor% Вы осторожно взяли удилище и резко цмыкнули.
wechoaround %actor% Увидев что клюет, %actor.name%, дрожащими руками дернул%actor.g% удилище.
calcuid uddochkk 33004 room
detach 33024 %uddochkk.id% 
switch %random.3%
  case 1
    wait 1s
    wsend       %actor% Здоровенная рыбина сорвалась с крючка... Обидно.
    wechoaround %actor%  %actor.name% крякнул%actor.g% от досады, когда огромная рыбина сорвалась с крючка.
    calcuid udfochkki 33004 room
    attach 33023 %udfochkki.id% 
    wpurge  fishhook330_2
    wload obj 33028
    calcuid uhdochkkk 33004 room
    detach 33025 %uhdochkkk.id% 
  end
break
case 2
  wait 1s
  wsend       %actor% Вы успешно подсекли рыбешку.
  wechoaround %actor%  %actor.name% с ехидной улыбкой вытащил%actor.g% маленькую рыбешку.
  calcuid ujdochkki 33004 room
  attach 33023 %ujdochkki.id% 
  wpurge  fishhook330_2
  wload obj 33003
  wload obj 33028
  calcuid uldochkkk 33004 room
  detach 33025 %uldochkkk.id% 
end
break
case 3
  wait 1s
  wsend       %actor% Вы успешно подсекли крупную рыбину.
  wechoaround %actor%  %actor.name%, покраснев от натуги, вытащил%actor.g% большую рыбину.
  calcuid uxdochkki 33004 room
  attach 33023 %uxdochkki.id% 
  wpurge  fishhook330_2
  wload obj 33034
  wload obj 33028
  calcuid udoqwchkkk 33004 room
  detach 33025 %udoqwchkkk.id% 
end
break
done
~
#33026
удаляем все с клетки~
2 z 100
~
wpurge  fishhook330
wpurge  fishhook330_2
wload obj 33028
~
#33027
приветствие~
0 r 100
~
wait 1
msend %actor% Лодочник сказал тебе:
msend %actor% - Здраве тебе, %actor.name%!
msend %actor% - Я могу перевезти тебя через реку к Переяславлю за 100 кун.
~
#33028
перевоз~
0 m 1
~
wait 1
switch %amount%
  case 100
    msend %actor% Лодочник неторопливо залез в лодку, затем Вам помог разместиться в ней.
    wait 1s
    msend %actor% Вы поплыли на лодке.
    msend %actor% Сначала через Днепр, а потом по небольшой речке впадающей с востока в Днепр.
    if %actor.sex%==1
      mechoaround %actor% %actor.name% залез в лодку с лодочником.
      mechoaround %actor% %actor.name% уплыл на лодке с лодочником.
    else
      mechoaround %actor% %actor.name% залезла в лодку с лодочником.
      mechoaround %actor% %actor.name% уплыла на лодке с лодочником.
    end
    wait 1s
    msend %actor% Вы приплыли к Переяславлю и вылезли из лодки.
    msend %actor% Лодочник сел в лодку и поплыл назад.
    mteleport %actor% 25065
    wait 1s
    * if %actor.sex%==1
    *  mat 25065 mechoaround %actor% %actor.name% приплыл с лодочником.
    *  mat 25065 mechoaround %actor% %actor.name% вылез из лодки.
    * else
    *  mat 25065 mechoaround %actor% %actor.name% приплыла с лодочником.
    *  mat 25065 mechoaround %actor% %actor.name% вылезла из лодки.
    * end
    *  mat 25065 mechoaround %actor% Лодочник сел в лодку и поплыл назад.
    *  wait 1s
    *  mat 33027 mecho Лодочник приплыл назад.
  break
  default
    msend %actor% Что-то не то.
    msend %actor% Я называл необходимую сумму, а это непонятно что. 
    дать %amount% %actor.name%
  break	
done
~
#33029
Лоад буки снять молчание с купца~
0 f 100
~
if %random.100% <= 15
  if %world.curobjs(562)% < 2
    mload obj 562
  end
end
~
#33030
Сдали мешок рыбачке~
0 j 100
~
wait 1
if ( %object.vnum% == 11407 )
  wait 1
  mpurge %object%
  if !%actor.quested(27002)% || %actor.quested(27003)%
    halt
  end
  %actor.unsetquest(27002)%
  %actor.setquest(27003)%
  say Так тебя батюшка прислал?
  mecho _- Ну, здрав%actor.g% будь, спасибо за помощь.
  взд
  wait 2s
  say Ох тяжко одной, без мужа.
  mecho _- Сама стараюсь - и по дому, да и жить чем-то надо.
  mecho _- Даже сети ставить пробовала - мужики насмех подняли.
  смущ
  wait 2s
  mecho _- А недавно последнюю сеть порвал кто-то...
  mecho _- Новую покупать придется.... за последние гроши.
  wait 2s
  emot посмотрела по сторонам
  wait 1s
  шепт %actor.name% Рыбаки сказывали, по ночам по реке водяной шастает.
  msend %actor% _- Он-то сети и рвет - гневится, что не поклонились ему.
  msend %actor% _- Страсти-то какие!
  крест
  шепт %actor.name% А кланяться ему поп запретил - бог, дескать, не простит.
  плюн
  взд
  wait 2s
  say Может ты нашему горю пособишь? Говорят - на острове, что посреди реки...
  emot глянула в окошко и неожиданно умолкла
  wait 1s
  emot занялась домашними делами, испуганно поглядывая на дверь
  halt
end
if ( %object.vnum% != 33035 ) && ( %object.vnum% != 33036 )
  drop all
  halt
end
if !%actor.quested(27005)% && ( %object.vnum% == 33036 )
  wait 1
  mpurge %object%
  say Это же сеть соседа моего - рыбака!
  emot Осмотрела вас с ног до головы.
  крича Ах ты, убивец!
  mecho Рыбачка схватила ухват и кинулась на вас в атаку!
  mkill %actor%
  halt
end
wait 1s
switch %object.vnum%
  case 33035
    wait 1
    mpurge %object%
    блед
    say Ужасть-то какая!
    say Справил%actor.u%-таки с ним!
  break
  case 33036
    wait 1
    mpurge %object%
    say Так кто сети рвал?
    mecho _- Рыбак, говоришь?!
    mecho _- Небось тот, что сам всем сети и делал за плату!
    mecho _- Ну, старик наш ему задаст...
  break
done
поклон .%actor.name%
say Спасибо тебе, %actor.name%!
mecho _- Услужи уж еще раз - передай от меня весточку батюшке.
mecho _- Век благодарна буду.
mload obj 33037
%actor.setquest(27007)%
дать весточ .%actor.name%
detach 33035 %self.id%
~
#33031
Квестор вошел к водяному~
0 q 100
~
wait 1
if !%actor.quested(27003)% || %actor.quested(27004)%
  halt
end
wait 1s
ворч
say Отроду людским духом тут не пахло...
wait 1s
mecho _- Ну, чего надобно?!
mecho _- Проваливай подобру-поздорову, а не то быть тебе лягушкой!
mecho _Глаза водяного засветились мрачноватым зеленым светом.
%actor.setquest(27004)%
~
#33032
Водяной ругается~
0 d 1
сети сеть~
wait 1
if !%actor.quested(27004)% || %actor.quested(27005)%
  halt
end
рыч
say Какие еще сети?! Не знаю я сетей ваших и не ведаю!
mecho _- Вечно найдут, какую напраслину возвести, то я сети рву, то коз дою по ночам!
плюн
say Сами же небось по пьяному делу и порвали, али рыбак тот, которого я давеча на берегу видал
mecho _- В овечью шубу вывороченную одетым - он и порвал.
mechp _- А зачем ему это - кто вас, людей, разберет!
wait 1s
рыч
say Проваливай, говорю!
мат
%actor.setquest(27005)%
wat 33011 wload mob 33035
wat 33011 wload mob 33035
~
#33033
Водяной убит~
0 f 100
~
if %actor.vnum% != -1
  set target %actor.leader%
else
  set target %actor%
end
if %target.quested(27005)% && ( %target.align% > 400)
  %target.align(-10)%
end
if %target.quested(27003)%
  mload obj 33035
end
~
#33034
Сдали сеть водяному~
0 j 100
~
wait 1
if !%actor.quested(27005)% || ( %object.vnum% != 33036 )
  рыч
  say Вон пош%actor.y%!!!
  бросить все
  halt
end
if %actor.quested(27006)%
  drop all
  halt
end
%actor.setquest(27006)%
wait 1s
say У того рыбака взял%actor.g%?
mecho _- Говорил же я, он сети и рвал!
взд
wait 2s
say Глядишь, теперь меня рыбаки тревожить реже станут...
say Как видно и от людей толк бывает.
челю
wait 2s
say Вот, завалялась у меня одна вещь, мне не нужна, а тебе, глядишь, пригодится.
switch %random.6%
  case 1
    mload obj 4413
  break
  case 2
    mload obj 550
  break
  default
    msend %actor% Водяной дал вам пригоршню кун.
    mechoaround %actor% Водяной дал %actor.dname% пригоршню кун.
    %actor.gold(+12000)%
  done
  дать все .%actor.name%
  say Что-то кости ломит - видать на берегу задержался...
  взд
  say Пойду, в реке порядок наведу.
  wait 1
  mpurge %self%
~
#33035
Сдали сеть или лапу рыбачке~
0 j 100
~
wait 1
if ( %object.vnum% != 33035 ) && ( %object.vnum% != 33036 )
  drop all
  halt
end
wait 1
mpurge %object%
if !%actor.quested(27005)% && ( %object.vnum% == 33036 )
  say Это же сеть соседа моего - рыбака!
  emot Осмотрела вас с ног до головы.
  крича Ах ты, убивец!
  mechoaround %actor% Рыбачка схватила ухват и кинулась на %actor.iname% в атаку
  msend %actor% Рыбачка схватила ухват и кинулась на вас в атаку!
  mkill %actor%
  halt
end
wait 1s
switch %object.vnum%
  case 33035
    блед
    say Ужасть-то какая!
    say Справил%actor.u%-таки с ним!
  break
  case 33036
    say Так кто сети рвал?
    mecho _- Рыбак, говоришь?!
    mecho _- Небось тот, что сам всем сети и делал за плату!
    mecho _- Ну, старик наш ему задаст...
  break
done
поклон .%actor.name%
say Спасибо тебе, %actor.name%!
mecho _- Услужи уж еще раз - передай от меня весточку батюшке.
mecho _- Век благодарна буду.
mload obj 33037
дать весточ .%actor.name%
detach 33035 %self.id%
~
#33036
Агрят пьяные рыбаки~
0 q 100
~
wait 1
if %direction% != north
  halt
end
if !%actor.quested(27005)%
  halt
end
ик
say Гляди.. Ик! Водхяной!
say Бей его.. з-заразу!
mkill %actor%
~
#33037
Убили ворюгу~
0 f 100
~
mload obj 33021
~
$~
