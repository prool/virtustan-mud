#38100
у старичка болотника~
0 h 100
~
wait 1
if %actor.vnum% == 27020
  mteleport %actor% 38143
  mecho Дочь воеводы дала маленькую горстку кун старичку-болотнику.
  mecho Старичок-болотник завязал дочери воеводы глаза, и повел на север.
  halt
end
wait 1s
foreach target %self.pc%
  msend %actor% Старичок довольно закряхтел, увидев Вас.
  msend %target% Старичок-болотник сказал : Если ты идешь на север, то берегись, там жуткие болота.
  msend %target% _- А я смог бы тебя проводить через них вполне безопасно.
  msend %target% _- Чтобы хоть как-то вознаградить мой тяжкий труд, не пожалей для
  eval needgold %target.level%*(%target.remort%+1)
  msend %target% _- старика %needgold% кун.
done
~
#38101
у мужичка болотника~
0 r 100
~
wait 1s
foreach target %self.pc%
  eval needgold 10+%target.level%*(%target.remort%+1)
  tell %target.name% Переправа через болота стоит %needgold% кун.
  tell %target.name% Если не хочешь платить - то иди туда сам%target.g%
done
ухм
~
#38102
дал бабки мужичку~
0 m 1
~
wait 1
eval needgold 10+%actor.level%*(%actor.remort%+1)
if %amount% < %needgold%
  дум
  say Нет, видно не судьба нам походить вместе...
  halt
end
wait 1
msend   %actor%  Мужичок-болотник поднялся и завязал вам глаза тряпицею.
mechoaround %actor%  Мужичок-болотник поднялся и завязал %actor.dname% глаза тряпицею.
msend   %actor% Он взял вас за руку и повел неведанными тропками.
mechoaround %actor%  %actor.name% и болотник ушли на юг.
wait 1
msend   %actor% В какой-то момент вам показалось, что он ходит кругами.
msend   %actor% Вы слышали странные крики и неприятные запахи болота.
msend   %actor% Вам казалось, что твари болотные наблюдают за вами.
mechoaround %actor% Болотник вернулся с юга, довольно подсчитывая барыши.
mteleport %actor% 38147 horse
*mat 38147  mechoaround %actor% _Кто-то пришел сюда со стороны болот.
msend   %actor% Но наконец путь кончился, и вы уже за болотами.
~
#38103
дал бабки старичку~
0 m 1
~
wait 1
eval needgold %actor.level%*(%actor.remort%+1)
if %self.fighting%
  give %amount% кун .%actor.name%
  halt
end
if %amount% < %needgold%
  wait 1
  дум
  if      %actor.sex% == 1
    say За такую сумму...иди ка ты молодец знаешь куда...
    msend   %actor% Старичок взмахнул руками, и вы оказались невесть где.
    mechoaround %actor% Старичок взмахнул руками, и  %actor.name% улетел невесть куда.
    mteleport %actor% 38168
    mat 38168  mechoaround %actor% _Кто-то пришел сюда со стороны болот.
    msend   %actor% Вы оказались на болоте.
    halt
  else 
    say Жадновата ты голубка...надо тебя проучить..
    msend   %actor% Старичок взмахнул руками, и вы оказались невесть где.
    mechoaround %actor% Старичек взмахнул руками, и  %actor.name% улетела невесть куда.
    mteleport %actor% 38168
    mat 38168  mechoaround %actor% _Кто-то пришел сюда со стороны болот.
    msend   %actor% Вы оказались на болоте.
    halt
  end
end
wait 1
if      %actor.sex% == 1
  msend   %actor%  Старик-болотник взмахнул рукой и у вас закружилась голова.
  mechoaround %actor%  Старик-болотник взмахнул рукой и  %actor.name%  исчез.
  mteleport %actor% 38143 horse
  mat 38143  mechoaround %actor% _Кто-то пришел сюда со стороны болот.
  msend   %actor% Вы оказались на дороге по ту сторону болот.
  halt
else
  msend   %actor%  Старик-болотник взмахнул рукой и у вас закружилась голова.
  mechoaround %actor%  Старик-болотник взмахнул рукой и  %actor.name%  исчезла.
  mteleport %actor% 38143 horse
  mat 38143  mechoaround %actor% _Кто-то пришел сюда со стороны болот.
  msend   %actor% Вы оказались на дороге по ту сторону болот.
  halt
end
~
#38104
лезть в бочку~
2 c 0
лезть пролезть залезть~
if (%actor.vnum% != -1)
  halt
end
if !(%arg.contains(бочка)%) 
  wsend %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
if !%actor.rentable%
  wsend %actor% Вы попытались залезть в бочку, но невиданная сила шарахнула вас огнем.
  %actor.hitp(1)%
  halt
end
calcuid bochka 38190 room
if %bochka.pc%
  wsend %actor% Там уже кто-то есть, а двое не поместятся - не селедки все-таки.
  halt
end
wsend %actor% Приоткрыв крышку, вы полезли в бочку с пивом..
wechoaround %actor% %actor.name% приоткрыл%actor.g% крышку и полез%actor.q% в бочку.
wait 1s
wsend %actor% .- Вы внутри, пахнет пивом и очень хочется солененькой рыбки.
wteleport %actor% 38190  
~
#38105
репопим тригера~
2 f 100
~
if (%random.1000% <= 250)
  calcuid zasada 38122 room
  detach 38110 %zasada.id%  
  calcuid zasada 38125 room
  detach 38112 %zasada.id%
  halt
end
switch %random.2%
  case 1
    calcuid zasada 38122 room
    attach 38110 %zasada.id%  
  break
  case 2
    calcuid zasada 38125 room
    attach 38112 %zasada.id%  
  break
done
~
#38106
лезть из бочки~
2 c 0
лезть пролезть залезть~
if !(%arg.contains(назад)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Приоткрыв крышку, вы полезли из бочки. Там и правда очень неудобно.
wait 1s
wsend %actor% .- Вы вылезли из бочки и все смотрят на вас с интересом.
wteleport %actor% 38129
wechoaround %actor% Кто-то вылез из бочки.
calcuid izbozku 38129 room
attach 38104 %izbozku.id%
detach 38107 %izbozku.id%
~
#38107
лезть в бочку когда полна~
2 c 0
лезть пролезть залезть~
if !(%arg.contains(бочка)%) 
  wsend       %actor% Куда это Вы хотите пролезть???
  return 0
  halt
end
wsend       %actor% Там уже кто-то есть, а двое не поместятся - не селедки все-таки.
~
#38108
лезть на холм~
2 c 0
лезть карабкаться залезть~
if !(%arg.contains(холм)%) 
  wsend       %actor% Куда это Вы хотите залезть???
  return 0
  halt
end
wsend       %actor% Вы полезли на холм.
wechoaround %actor% %actor.name% полез%actor.q% на холм.
wait 1s
wsend %actor% .- Не долго ж вам ногти ломать, и вот вы наверху.
wteleport %actor.name% 38176
wat %actor.realroom% wechoaround %actor% Кто-то прилез сюда снизу.
~
#38109
прыгнуть с холма~
2 c 0
прыгнуть спрыгнуть~
if !(%arg.contains(вниз)%) 
  wsend       %actor% Куда это Вы хотите прыгнуть???
  return 0
  halt
end
wsend       %actor% Осторожно придерживая одежду, вы прыгнули с холма.
wechoaround %actor% %actor.name% спрыгнул%actor.g% вниз.
wait 1s
wsend %actor% .- Вы оказались на дороге у подножия холма.
wteleport %actor.name% 38143
wat %actor.realroom% wechoaround %actor% Кто-то спрыгнул сюда с холма.
~
#38110
засада 38122~
2 e 100
~
wdamage %actor% 9
wsend       %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись вам в живот!!!!
wechoaround %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись живот %actor.rname%!!!!
wdamage %actor% 9
wload mob 38112
wload mob 38113
calcuid glavar 38112 mob
attach 38111 %glavar.id%
exec 38111 %glavar.id%
calcuid zasada 38122 room
detach 38110 %zasada.id%  
~
#38111
главарь атакует~
0 z 100
~
mkill %actor%
кри Бей их! За мной, ребята!!!
calcuid glavar 38112 mob
detach 38111 %glavar.id%
~
#38112
засада 38125~
2 e 100
~
wdamage %actor% 9
wsend       %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись вам в живот!!!!
wechoaround %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись живот %actor.rname%!!!!
wdamage %actor% 9
wload mob 38112
wload mob 38113
calcuid glavar 38112 mob
attach 38111 %glavar.id%
exec 38111 %glavar.id%
calcuid zasada 38125 room
detach 38112 %zasada.id%
~
#38113
помер холоп~
0 f 100
~
if (%world.curobjs(38100)% < 5) && (%random.4% == 1)
  mload obj 38100
end
~
#38114
помер главарь~
0 f 100
~
if ((%world.curobjs(38101)% < 1) && (%random.1000% <= 100))
  mload obj 38101
end
~
#38115
помер вурнака~
0 f 100
~
*if (%world.curobjs(613)% < 30) && (%random.4% == 1)
*   mload obj 613
*end
*if (%world.curobjs(614)% < 30) && (%random.4% == 1)
*   mload obj 614
*end
~
#38116
помер змий~
0 f 100
~
if (%world.curobjs(541)% < 1) && (%random.4% == 1)
  mload obj 541
end
~
#38117
приходит сюда игрок с проводником~
2 z 100
~
wechoaround %actor% _Кто-то пришел сюда со стороны болот.
~
#38118
Нападение урода на квестера ГФ~
2 e 100
~
wait 1
*%actor.wait(4)%
foreach char %self.pc%
  if %char.haveobj(11407)% && !%char.quested(27013)%
    %char.setquest(27012)%
    wecho Страшно уродливый человек вдруг выскочил из кустов!
    wsend %char%  Оторопев от такого зрелища, вы выронили мешок с гостинцами от старого охотника...
    wechoaround %char% %char.name% шарахнул%char.u% в сторону и выронил%char.g% мешок.
    wforce %char% drop questitem11401
    wpurge questitem11401
    wecho Урод подхватил мешок и зайцем припустил по дороге на восток.
    halt
  end
done
~
#38119
Убит купец Бдигост (квест на огненную стрелу)~
0 f 100
~
foreach killer %self.pc%
  if %killer.quested(38102)% && (%killer.align% > 200)
    %killer.align(-5)%
  end
done
mload obj 38105
~
#38120
Квестер входит к купцу~
0 q 100
~
wait 1
if %self.fighting%
  halt
end
wait 1s
mecho Бдигост оглядел Вас с головы до пят.
if !%actor.quested(33902)% || %actor.quested(33905)%
  halt
end
say Ты от травника? Что ж он сам не пришел? 
if ( %world.curobjs(38105)% >= 1 )
  say Приходил уже от него человек - и вещичку ту я ему отдал.
  say Не обессудь.
  halt
end
%actor.setquest(33905)%
mecho _- А то разговор у меня к нему серьезный, денежный.
mecho _- Свиток я привез, да вот потратиться пришлось изрядно...
wait 2s
say Так что причитается с него. Ты сам решай - сейчас деньги отдашь, или к нему сбегаешь,
mecho _- Но заплатить придется - пять тысяч кун, до последнего резана!
~
#38121
Заплатили купцу~
0 m 1
~
wait 2
if !%actor.quested(33905)%
  дум .%actor.name%
  say Ты чего это, мил-человек?
  say У нас с тобой никаких уговоров не было, ступай-ка своей дорогой.
  give %amount% кун .%actor.name%
  halt
end
if ( %world.curobjs(38105)% >= 1 )
  say Сказано же тебе, нет отдал я уже свиток посланному! 
  say Ступай.
  give %amount% кун .%actor.name%
  halt
end
mecho Купец пересчитал деньги.
if %amount% < 5000
  mecho Купец пересчитал деньги.
  хмур
  wait 2
  say Надуть хочешь? Нехорошо, право слово.
  say Плати как уговорено - или ступай отсель.
  give %amount% кун .%actor.name%
  halt
end
%actor.setquest(33906)%
mload obj 38105
дать свиток .%actor.name%
say Вот... Ты не думай, я не жадный, просто поиздержался в пути. 
say А свиток-то тоже немалых денег стоит, а? Слышал я о ваших делах местных!
emot лукаво усмехнулся
~
$~
