#167100
начало квеста~
0 q 100
~
вздох
гов здравствуй, путник
wait 1s
гов произошла у меня беда страшная, как и жить мне дальше не знаю
грус
wait 1s
гов был я корованщиком богатым, торговал тканями заморскими, украшениями драгоценными, да яствами изысканными
гов были у меня коровы самые породистые, нигде больше таких не найти
гов скопил я столько погатства, что отстроил себе в киеве замок громадный
wait 1s
гов и всё это пропало в один день!
рыд
wait 1s
гов Номад, этот чёртов разбойник, будь он проклят во веки веков!
гнев
wait 1s
гов был мне заказ от самого князя киевского, мол жениться дочь его хотела, а князь пожелал, чтоб приданое ее было самым дорогим да богатым во всех известных землях
гов ходил я по свету тринадцать месяцев, три недели, 19 часов, да еще к тому шестьдесят девять минут
гов когда договором, а когда и обманом получая то, что было нужно князю
wait 1s
гов и был собран корован, выдвинулись мы в путь, но вдруг
гов я услышал отчаянный крик: набигаем!!!
гов и сразу же после этого откуда не возьмись появились перед нами деревянные домики, непонятно как передвигающихся, а возглавлял их Номад
wait 1s
гов Номад- этот разбойник.
вздох
гов он держал в страхе всех корованщиков на руси, ходили даже слухи, что он смог ограбить легендарный черный корован
гов я посмотрел на него: он был одет в какое-то рванье, на плече его была странная железная палка, а лицо было искажено в приступе дикой, неконтролируемой злобы
wait 1s
гов я понял: нам не жить
гов они набигали так стремительно, так беспощадно убивали наших коров, так уверенно грабили наши мешки с добром
дум
wait 5s
гов путник, принеси мне голову этого выродка и я отблагодарю тебя
гов хоть у меня почти ничего и не осталось, но я отдам тебе древний припрятанный неподалеку артефакт, который дарует свежесть, прохладу и возможно силу земли
attach 167101 %self%
detach 167100 %self%

~
#167101
согласие на квест~
0 d 1
да помогу согласен принесу хорошо сделаю выполню подсоблю~
eval utime %date.unix%
eval time1 %actor.getquest(1671)%
eval time2 (%utime% -%time1%)/60
if !%tmp%
  eval tmp 1
  global tmp
  eval time3 59+%random.181%
  global time3
end
if %time2% >= %time3% || !%actor.quested(1671)%
  гов не подведи меня, путник
  calcuid zasada 167101 room
  attach 167102 %zasada%
  attach 167105 %self%
  eval player %actor%
  global player
  detach 167101 %self%
else
  eval time4 (%time3%-%time2%)/2
  гов нет, мне пока не нужна твоя помощь, подходи где-то через %time4% часов
end

~
#167102
игрок прошел по дороге~
2 g 100
~
wait 1s
%load% mob 167101
wat 167199 %load% mob 167102
calcuid nomad 167102 mob
calcuid domiki 167101 mob
%force% %nomad% крич набигаем!!!
wait 15s
%load% mob 167103
%load% mob 167103
%load% mob 167103
%echo% внезапно выбежали еще несколько домиков, которые присоединились к остальным
detach 167102 %self%

~
#167103
смерть домиков~
0 f 100
~
calcuid nomad 167102 mob
%teleport% %nomad% 167101
%force% %nomad% гов да я целые корованы грабил, что мне вы то
%force% %nomad% плю
%force% %nomad% убить %actor%

~
#167104
смерть номада~
0 f 100
~
%load% obj 167100

~
#167105
отдаем голову корованщику~
0 j 100
~
wait 1
if %object.vnum% != 167100
  гов зачем мне это? мне такого не нужно
  бросить все
else
  if %actor% != %player%
    гов не просил я тебя о помощи, стыдно чужие заслуги присваивать
    mjunk all
  else
    гов спасибо тебе, добрый человек, во век не забуду
    рад %player.name%
    гов что же касаемо награды
    дум
    %actor.setquest(1671 %date.unix%)%
    фиг %player.name%
    mjunk all
  end
end
%echo% корованщик ушел дальше по дороге
%purge% %self%

~
#167106
ресет зоны~
2 f 100
~
calcuid quester 167100 mob
attach 167100 %quester%

~
#167107
получение квесточков~
2 c 100
квест~
if !%%actor%,qp%
  set %actor%,qp
  remote %actor%,qp %actor%
end
%send% %actor% текущее количество квесточков: %%actor%,qp%
%send% %actor% бонус, 10 квесточков!
set %actor%,qp %%actor%,qp% +10
global %actor%,qp
%send% %actor% стало квесточков: %%actor%,qp%

~
#167108
проверка заработка казино~
2 c 100
сколько~
if %actor.name% != дариан
  return 0
  halt
else
  %send% %actor% казино заработало %balans% кун и %obalans% опыта
end

~
#167109
забрать заработок~
2 c 100
забрать~
if %actor.name% == Дариан || %actor.name% == Огнеслав
  if %arg.car% == куны
    eval num %arg.words(2)%
    %send% %actor% вы решили забрать %num% кун
    if %num% < 1
      %send% %actor% но ведь так же нельзя
    elseif %num% > %balans%
      %send% %actor% ваше казино столько не выиграло, увы
      halt
    end
    eval balans %balans% -%num%
    global balans
    eval num2 %actor.gold(+%num%)%
    %send% %actor% вы забрали %num% кун из казино
    %send% %actor% стало кун у вас: %num2%, осталось кун в казино: %balans%
  elseif %arg.car% == опыт
    eval num %arg.words(2)%
    %send% %actor% вы решили забрать %num% опыта
    if %num% < 1
      %send% %actor% но ведь так же нельзя
    elseif %num% > %obalans%
      %send% %actor% ваше казино столько не выиграло, увы
      halt
    end
    eval obalans %obalans% -%num%
    global obalans
    eval num2 %actor.exp(+%num%)%
    %send% %actor% вы забрали %num% опыта из казино
    %send% %actor% стало опыта у вас: %num2%, осталось опыта в казино: %obalans%
    halt
  end
else
  %send% %actor% Чаво?
  halt
end

~
#167110
блокировка умений~
1 c 100
*~
eval cm %cmd.mudcommand%
if %cm% == конец || %cm% == quit || %cm% == постой || %cm% == rent
  %send% %actor% ну уж нет, так просто сбежать не выйдет
  return 0
elseif %cm% == болтать || %cm% == кричать || %cm% == орать || %cm% == оффтоп || %cm% == воззвать || %cm% == сказать || %cm% == ответить || %cm% == спросить || %cm% == tell || %cm% == shout || %cm% == reply
  
  
  
  
  
  
  
  
  
  
  
end

~
#167111
простейший триг на бессмертие моба~
0 f 100
~
%load% mob %self.vnum%
%load% mob %self.vnum%

~
#167112
призыв убийцы~
1 c 2
сломать~
if %arg.contains(печать)%
  %send% %actor% вы сломали печать, открылся портал, из которого вышел наемный убийца. осталось только сказать ему, как зовут жертву.
  %load% mob 167008
else
  %send% %actor% ну и ломайте себе на здоровье, но что?
end

~
#167113
тех.работы~
1 c 1
техработы~
calcuid rroom %actor.realroom% room
if %arg.contains(вкл)%
  %send% %actor% запуск технических работ, доступ в локацию закрыт
  attach 167114 %rroom%
elseif %arg.contains(выкл)%
  %send% %actor% технические работы завершены, доступ в комнату открыт
  detach 167114 %rroom%
else
  %send% %actor% вкл/выкл
end

~
#167114
тех.работы2~
2 g 100
~
%send% %actor% идут технические работы, доступ в локацию запрещен
return 0

~
#167115
приказ кому угодно~
1 c 1
приказ~
eval prikaz %arg.cdr%
eval str %arg.car%
eval victim %str.id%
if %victim.name% == Дариан
  %send% %actor% да ты нарываешься, божок
  halt
elseif %victim.realroom% < -1
  %send% %actor% ты еще пустоте поприказывай, умник
  halt
else
  %send% %actor% ты приказал%actor.g% %victim.dname% сделать %prikaz%
  attach 167116 %victim%
  exec 167116 %victim%
  detach 167116 %victim%
end

~
#167116
выполнение приказа~
0 z 100
~
%prikaz%

~
#167117
повернуть кольцо~
1 c 1
повернуть~
if %arg.contains(кольцо)%
  %send% %actor% вы повернули на пальце кольцо белого духа и почувствовали, как прохладная волна окатила вас, излечивая все раны
  eval hp %actor.hitp(%actor.maxhitp%)%
  %send% %actor% стало жизни: %hp%
  %actor.wait(5)%
else
  %send% %actor% тут была пошлая шутка, но ее заставили убрать  тогда просто спрошу: что поворачивать?
end

~
#167118
бросить колбу~
1 h 100
~
foreach i %actor.all%
  if %i% == %actor%
    %send% %actor% вы бросаете колбу, она разбивается, и из нее вырывается пламя
    %echoaround% %actor% %actor.name% бросает какую-то колбу, она разбивается, и из нее вырывается пламя
  else
    wait 1s
    dg_cast 'гнев' %i%
  done
end
%purge% %self%

~
#167119
жертва~
1 c 1
жертва~
%send% %actor% собравшись с силами, вы упираете глефу ужаса в землю и убиваете себя, проткнув острием свое сердце
%send% %actor% перед смертью вы успеваете увидеть кровавый туман, стремительно разростающийся от оружия и поглощающий всех вокруг
foreach i %actor.all%
  %damage% %i% 1000000
done

~
#167120
не снять глефу~
1 l 100
~
%send% %actor% вы попытались убрать глефу, но вдруг вокруг ваших рук возникли призрачные крючья, которые впились в кожу и кости, не давая этого сделать
%damage% %actor% 200
return 0

~
#167121
прикрепить триггер~
1 c 1
прикрепить~
eval arg1 %arg.cdr%
eval victim %arg1.id%
if %victim.name% == Дариан
  %send% %actor% не, нельзя так делать
else
  attach %arg.car% %victim%
  %send% %actor% вы прикрепили триггер %arg.car% к %victim.dname%
end

~
#167122
открепить триггер~
1 c 1
открепить~
eval arg1 %arg.cdr%
eval victim %arg1.id%
if %victim.name% == Дариан
  %send% %actor% не, нельзя так делать
else
  detach %arg.car% %victim%
  %send% %actor% вы открепили триггер %arg.car% от %victim.rname%
end

~
#167123
пук~
0 ab 100
пук~
пук

~
#167124
задача с тараканом~
2 c 100
решить~
set mine 70
set speed 10
eval down 20/3
set hours 0
set done 0
while %hours% <= %mine%
  eval hours %hours% + 1
  global hours
  eval done %done% +%speed%
  global done
  if %done% >= %mine%
  break
end
eval done %done% -%down%
global done
done
%send% %actor% понадобится часов: %hours%

~
#167125
игра угадывает число~
2 c 100
начнем~
eval player %actor%
global player
%send% %actor% я угадаю ваше загаданное число от 1 до 1000 за 11 или менее попыток, на каждое мое предложение пишите больше, меньше или равно
eval min 1
eval max 1000
global min
global max
eval num (%min% +%max%)/2
global num
eval try 1
global try
%send% %actor% попытка %try%,  загаданное число %num%
attach 167126 %self%

~
#167126
игрок озвучивает результат~
2 c 100
*~
if %actor% == %player%
  if %num% == 0 || %try% > 11
    %send% %actor% вы меня обманывали, не стыдно?
    attach 167125 %self%
    detach 167126 %self%
    halt
  end
  if %cmd% == меньше
    eval max %num%
    global max
  elseif %cmd% == больше
    eval min %num%
    global min
  elseif %cmd% == равно
    %send% %actor% ура, я угадала
    attach 167125 %self%
    detach 167126 %self%
  else
    return 0
    halt
  end
  if %min% == 999
    eval min %min% +1
    global min
  end
  eval num (%min% +%max%)/2
  global num
  eval try %try% + 1
  global try
  %send% %actor% попытка %try%, загаданное число %num%
else
  return 0
  halt
end

~
#167127
моделирование игры в казино~
2 c 100
игра~
eval money %arg.car%
eval bid %arg.words(2)%
eval try %arg.words(3)%
eval allgame 0
eval goodgame 0
eval badgame 0
eval min %arg.words(4)%
eval max %arg.words(5)%
eval vin %arg.words(6)%
eval factor %arg.words(7)%
if %money% < 1 || %try% < 1 || %try% > 1000 || %bid% < 1 || %min% > %max% || %vin% < %min% || %vin% > %max% || %factor% < 1
  %send% %actor% запас у.е и ставка не могут быть меньше 1, множитель не может быть выше 2, количество попыток не может быть меньше 1 и больше 1000, минимальное рандомное число не может быть выше максимального, показатель победы не может быть ниже мин
  halt
end
%send% %actor% моделирование игровой ситуации: баланс- %money%, ставка- %bid%, попыток сыграть- %try%, рандом %min%- %max%, победа если рандом <= %vin%, множитель победы- %factor%
while %try% > 0
  if %money% < 1
    eval money 0
    global money
  break
end
if %bid% > %money%
  eval bid %money%
  global bid
end
if %number.range(%min%,%max%)% <= %vin%
  eval money %money% +(%bid%*%factor%)
  global money
  eval goodgame %goodgame% +1
  global goodgame
else
  eval money %money% -%bid%
  global money
  eval badgame %badgame% +1
  global badgame
end
eval allgame %allgame% +1
eval try %try% -1
global try
done
%send% %actor% результат:
%send% %actor% сыграно всего игр- %allgame%, выигрышных- %goodgame%, проигрышных- %badgame%
%send% %actor% баланс- %money%

~
#167128
кастомный рандомайзер~
1 c 2
кость~
eval min %arg.car%
eval max %arg.words(2)%
eval dices %arg.words(3)%
set mod %arg.words(4)%
eval sum 0
if !%min%
  eval min 0
end
if !%max%
  eval max 0
end
if %mod% == 0
  set mod +0
end
if %dices% == 0
  eval dices 1
end
if %max% < %min% || %dices% > 1000
  %send% %actor% максимальное число не может быть меньше минимального, количество бросков не может быть больше 1000
  halt
end
%send% %actor% вы кинули кости от %min% до %max% %dices% раз с бонусом %mod%
%echoaround% %actor% %actor.name% кинул%actor.g% кости от %min% до %max% %dices% раз с бонусом %mod%
while %dices% > 0
  eval rnd %number.range(%min%,%max%)%
  set dropped %dropped% %rnd%,
  eval sum %sum% +%rnd%
  global sum
  eval dices %dices% -1
  global dices
done
eval total %sum% %mod%
%echo% выпало: %dropped%
%echo% всего выпало: %sum%, бонус: %mod%, итог: %total%

~
#167129
создание~
1 c 1
создать~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval flag 0
if %arg1% == вещь
  eval arg1 obj
  global arg1
  if %world.object(%arg1%)% != 0
    eval flag 1
    global flag
  end
elseif %arg1% == моба
  eval arg1 mob
  global arg1
  if %world.mob(%arg2%)% != 0
    eval flag 1
    global flag
  end
else
  %send% %actor% создать можно вещь или моба
  halt
end
if %flag% == 1
  %load% %arg1% %arg2%
  calcuid tmp %arg2% %arg1%
  %send% %actor% вы создали %tmp.vname%
  %echoaround% %actor% %actor.name% создал%actor.g% %tmp.vname%
else
  if %arg1% == obj
    %send% %actor% такой вещи не существует
  elseif %arg1% == mob
    %send% %actor% такого моба не существует
  end
end
~
#167130
истинный огонь~
1 c 1
отчистить~
eval rroom %world.room(%arg%)%
eval fr %rroom.firstvnum%
if !%rroom.vnum%
  %send% %actor% такой комнаты не найдено!
  halt
end
eval lastr %rroom.lastvnum%
while %fr% <= %lastr%
  eval rroom %world.room(%fr%)%
  if !%rroom.vnum%
    *nop
  else
    foreach i %rroom.all%
      %send% %i% повсюду прошлась волна истинного пламени, выжигая абсолютно все на своем пути
      %damage% %i% 2000000000
    done
    foreach i %rroom.objects%
      %purge% %i%
    done
    foreach i %rroom.objects%
      %purge% %i%
    done
    %send% %actor% комната %rroom.vnum% (%rroom.name%), отчистка прошла успешно
  end
  eval fr %fr% +1
  global fr
  wait 1
done

~
#167131
сейф~
2 c 100
сейф~
if %arg.car% == пароль
  if %actor.name% == Дариан
    eval pass %arg.cdr%
    global pass
    %send% %actor% установлен пароль %pass%
  else
    %send% %actor% да вы кем себя возомнили?
    halt
  end
elseif %arg.car% == ввести
  if !%try%
    eval try 3
  end
  if %arg.cdr% != %pass%
    %send% %actor% осталось попыток: %try%
    eval try %try% -1
    global try
    if %try% < 1
      %send% %actor% самоуничтожение, активация
      eval %try% 3
      global try
      foreach i %actor.all%
        %damage% %i% 2000000000
        halt
      end
    done
  elseif %arg.cdr% == %pass%
    %send% %actor% сейф раскрылся, роняя на пол свое содержимое
    %load% obj 167055
    %load% obj 167056
    %load% obj 167057
    %load% obj 167057
    %load% obj 167058
    %load% obj 167058
    %load% obj 167059
    %load% obj 167059
    %load% obj 167060
    %load% obj 167061
    %load% obj 167062
    %load% obj 167063
  end
else
  %send% %actor% пароль- установить новый пароль, ввести- попытаться ввести установленный пароль
end

~
#167132
активация контракта~
1 c 2
отогнуть~
if %arg.contains(уголок)%
  %send% %actor% вы отогнули уголок на контракте, после чего он рассыпался в ваших руках, а перед вами из тени возник даже свиду опасный человек.
  %send% %actor% осталось только назвать имя цели- подумали вы, вспомнив, что было написано в контракте
  %load% mob 167008
  %purge% %self%
else
  %send% %actor% запросто, а что?
end

~
#167133
заказ цели~
0 d 100
*~
eval lvl %actor.level% +10
eval victim %speech.id%
if %victim.vnum% != -1
  гов да ты за кого меня принимаешь? думаешь я на тварей неразумных охотиться буду?
  halt
end
if %victim.realroom% < 1
  гов нету сейчас в мире этой цели
  halt
end
if %victim.level% > %lvl
  гов а не маловат%actor.g% ты таких серьезных людей заказывать?
  ржать %actor.name%
  halt
end
if %victim.level% > 30
  гов вроде уже %actor.age% годков стукнуло, а ума то не прибавилось: боги бессмертны!
  halt
end
wait 1
гов один момент
eval player %actor%
global player
global victim
eval rroom %victim.realroom%
%echo% %self.name% скрыл%self.u% в тенях
while %victim.hitp% > 0
  if %victim.fighting% != %self%
    eval rroom %victim.realroom%
    %teleport% %self% %rroom%
    %echo% %self.name% возник%self.q% из тени
    mkill %victim%
  end
  wait 3s
done

~
#167134
наемник выполнил заказ~
0 o 100
~
if %actor.name% == %victim.name%
  wait 1s
  взять труп.%victim.rname%
  %teleport% %self% %player.realroom%
  wait 1
  шеп %player.name% заказ выполнен
  дать все.труп %player.name%
  wait 1s
  %echo% не успели вы моргнуть и глазом, как наемник куда-то исчез
  %purge% %self%
end

~
#167135
наемник не выполнил заказ~
0 f 100
~
if %player% != 0 && %victim% != 0
  %echo% из последних сил %self.name% приподнял%self.u% и вдруг исчез%self.q%
  %teleport% %self% %player.realroom%
  %echo% из теней вывалил%self.u% окровавленн%self.w% %self.name%
  гов увы, я не справил%self.u%
end

~
#167136
получение гипноза~
2 c 100
учить~

~
#167137
смена комнаты~
1 c 1
комната~
eval arg1 %arg.car%
eval victim %arg1.id%
if %victim.name% == Дариан
  %send% %actor% поцелуй меня в задницу, ага
else
  %victim.room(%arg.cdr%)%
  eval rroom %world.room(%victim.realroom%)%
  %send% %actor% у %victim.rname% текущая комната установлена в %rroom.vnum% ( %rroom.name% )
  %send% %victim% внезапно у вас закружилась голова, помутилось в глазах, но через мгновение это прошло
end

~
#167138
калькулятор~
2 c 100
считать~
set calc %arg%
eval equals %calc%
%send% %actor% %calc% =%equals%

~
#167139
покупка товара в магазине~
0 z 100
~
* все данные будут получены из вызывающего триггера
* создаем необходимые переменные и условия к ним
eval item %arg.car%
eval maxitem %objects.words%
if %speech_no_object% == 0
  set speech_no_object я такого не продаю
end
if %item% > %maxitem% || %item% < 1
  гов %speech_no_object%
  halt
end
eval multy %arg.words(2)%
if %multy% == 0
  eval multy 1
end
eval object %array.item(%objects%, %item%)%
eval price %array.item(%prices%, %item%)%
* проверяем, может ли чар купить вещь
if %actor.gold% < %price%
  if %speech_no_gold% == 0
    set speech_no_gold но веть у вас нет столько денег, сколько стоит этот товар
  end
  гов %speech_no_gold%
else
  calcuid uobj %object% obj
  %send% %actor% вы решили купить %uobj.vname% в количестве %multy% штук и протянули деньги
  * запускаем цикл покупки предметов, затем отдаем их
  eval count %multy%
  while %count% > 0
    if %actor.gold% < %price%
    break
  end
  %actor.gold(-%price%)%
  %load% obj %object%
  eval count %count% -1
  eval sum %sum% +1
  global count
  global sum
done
if %sum% < %multy%
  гов этих денег хватило только на %sum% предметов
else
  if %speech_enough_gold% == 0
    set speech_enough_gold вот, держите
  end
  гов %speech_enough_gold%
  eval nobj %uobj.name%
  дать все.%nobj.car% %actor.name%
  eval sum 0
end
end
~
#167140
пример магазина~
0 c 100
*~
eval objects 100 101 102
eval prices 5 10 15
set speech_no_object разуй глаза, я не продаю такого
set speech_no_gold нищеброд, нету у тебя голды столько
set speech_enough_gold во, хоть один нормальный клиент, держи конечно
eval cm %cmd.mudcommand%
if %cm% == список
  attach 167141 %self%
  exec 167141 %self%
  detach 167141 %self%
elseif %cm% == купить
  attach 167139 %self%
  exec 167139 %self%
  detach 167139 %self%
elseif %cm% == информация
  %send% %actor% купить номерпредмета количество
  %send% %actor% список
  %send% %actor% информация
else
  return 0
end

~
#167141
вывод списка товаров~
0 z 100
~
eval num 1
eval maxitem %objects.words%
%send% %actor% номер: предмет - цена (в кунах)
while %num% <= %maxitem%
  eval objarray %array.item(%objects%, %num%)%
  calcuid uobj %objarray% obj
  %send% %actor% %num%: %uobj.iname% - %array.item(%prices%, %num%)%
  eval num %num% + 1
done

~
#167142
игровой автомат~
2 c 100
автомат~
eval bet %arg.car%
if %bet% > %actor.gold%
  %send% %actor% вы судорожно стали шарить по карманам, в надежде отыскать столько, сколько пожелали поставить, но эти поиски не увенчались успехом.
  halt
end
if %bet% < 1
  %send% %actor% даже интересно стало: и как вы себе это представляете?
  halt
end
set l яблоко груша банан лимон апельсин вишня клубника арбуз
eval size %l.words%
eval a %array.item(%l%, %random.num(%size%)%)%
eval b %array.item(%l%, %random.num(%size%)%)%
eval c %array.item(%l%, %random.num(%size%)%)%
%actor.gold(-%bet%)%
%send% %actor% вы решили сыграть на %bet% кун и дернули рычаг. барабаны завертелись.
%send% %actor% %a%
%send% %actor% %b%
%send% %actor% %c%
wat 167023 %echo% %actor.name% дернул%actor.g% рычаг, и барабаны завертелись.
wat 167023 %echo% %a%
wat 167023 %echo% %b%
wat 167023 %echo% %c%
if %a% == клубника || %b% == клубника || %c% == клубника || %a% == банан || %b% == банан || %c% == банан
  eval bonus %bet%/3
  %send% %actor% внимание! вы выиграли бонус: %bonus% кун!
  wat 167023 %echo% %actor.name% выиграл%actor.g% бонус в размере %bonus% кун
  %actor.gold(+%bonus%)%
end
if %a% == вишня || %b% == вишня || %c% == вишня
  eval bonus (%bet%/3)*2
  %send% %actor% внимание! вы выиграли бонус: %bonus% кун!
  wat 167023 %echo% %actor.name% выиграл%actor.g% бонус в размере %bonus% кун
  %actor.gold(+%bonus%)%
end
if %a% == апельсин || %b% == апельсин || %c% == апельсин || %a% == арбуз || %b% == арбуз || %c% == арбуз
  eval bonus (%bet%/5)*2
  %send% %actor% внимание! вы выиграли бонус: %bonus% кун!
  wat 167023 %echo% %actor.name% выиграл%actor.g% бонус в размере %bonus% кун
  %actor.gold(+%bonus%)%
end
eval jbank %jbank% +(%bet%/2)
global jbank
if %a% == %b% && %a% == %c% && %b% == %c%
  eval rnd %number.range(2, 5)%
  %send% %actor% ура! вы выиграли, ставка увеличина в %rnd% раз!!!
  wat 167023 %echo% %actor.name% выиграл%actor.g% %vin% кун
  eval vin %bet%*%rnd%
  %actor.gold(+%vin%)%
  eval jbank %jbank% +(%vin%/4)
  global jbank
  eval jp1 %random.25%
  if %jp1% == 1
    eval jpmulty %number.range(5, 20)%
    eval jpvin %vin%*%jpmulty%
    %send% %actor% вы сорвали джекпод, выигрыш увеличивается в %jpmulty% раз и становится равен %jpvin%! наши искренние поздравления!
    wat 167023 %echo% %actor.name% получает %jpvin% кун, сорвав джекпод!!!
    %actor.gold(+%jpvin%)%
    %actor.gold(-%vin%)%
    eval jbank %jbank% +(%jpvin%/4)
    global jbank
    eval jp2 %jp2% +%random.20%
    global jp2
  end
  if %jp2% > 99
    %send% %actor% вы срываете ультраджекпод и забираете весь банк, размер которого %jbank% кун!!! круче вас только яйца.
    wat 167023 %echo% %actor.name% срывает ультраджекпод и забирает весь накопленный банк в размере %jbank% кун!!!
    %actor.gold(+%jbank%)%
    eval jbank 0
    global jbank
    eval jp2 0
    global jp2
  end
end

~
#167143
опознание вещи в магазине~
0 z 100
~
eval item %arg.car%
eval object %array.item(%objects%, %item%)%
eval nobj %object.name%
eval word %nobj.car%
if %item% > %objects.words% || %item% < 1
  if %speech_no_identify%
    set speech_no_identify у меня нету того, о чем ты хочешь узнать
  end
  гов %speech_no_identify%
  halt
end
%load% obj %object%
calcuid uobj %object% obj
%uobj.put(%actor%)%
%spellturntemp% %actor% полная 1
%force% %actor% колд !полная! %word%
%purge% %uobj%
end

~
#167144
узнаём время~
2 c 100
тест~
%send% %actor% игровое время:
%send% %actor% %time.hour% часов, %time.day%.%time.month%.%time.year%
%send% %actor% реальное время:
%send% %actor% %date.hour%:%date.minute%, %date.day%.%date.month%.%date.year%, от 1970 года прошло %date.unix% секунд
version
mod тест
log тест
cmd
command
commands

~
#167145
таймер~
2 c 100
таймер~
eval time %arg.car%
eval interval %arg.words(2)%
if %time% < 1 || %interval% < 1
  %send% %actor% время таймера и интервал уведомлений не могут быть ниже 1
  halt
end
%send% %actor% запущен таймер на %time% секунд, оповещения- каждые %interval% секунд
attach 167146 %self%
while %time% > 0
  if %count% == %interval%
    %send% %actor% осталось %time% секунд
    eval count 0
    global count
  end
  wait 1s
  eval time %time% -1
  eval count %count% + 1
done
%send% %actor% время вышло

~
#167146
стоп таймера~
2 c 0
таймер стоп~
%send% %actor% таймер остановлен
detach 167145 %self%
attach 167145 %self%
detach 167146 %self%

~
#167147
изменение алигна~
1 c 1
наклонности~
set arg1 %arg.car%
set arg2 %arg.words(2)%
set arg3 %arg.words(3)%
eval victim %arg1.id%
%victim.align(%arg2%%arg3%)%
%send% %actor% у %victim.rname% наклонности установлены в %victim.align%
~
#167148
симулятор хождения~
2 c 100
*~
if !%flag%
  eval x 0
  eval y 0
  eval z 0
  eval flag 1
  global x
  global y
  global z
  global flag
  remote x %actor%
  remote y %actor%
  remote z %actor%
end
eval cm %cmd.mudcommand%
if %cm% == север || %cm% == north
  if %x% < 0
    eval x %x% -1
    global x
    remote x %actor%
  else
    eval x %x% +1
    global x
    remote x %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == юг || %cm% == south
  if %x% < 0
    eval x %x% + 1
    global x
    remote x %actor%
  else
    eval x %x% -1
    global x
    remote x %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == запад || %cm% == west
  if %y% < 0
    eval y %y% +1
    global y
    remote y %actor%
  else
    eval y %y% -1
    global y
    remote y %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == восток || %cm% == east
  if %y% < 0
    eval y %y% -1
    global y
    remote y %actor%
  else
    eval y %y% +1
    global y
    remote y %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == вверх || %cm% == up
  if %z% < 0
    eval z %z% -1
    global z
    remote z %actor%
  else
    eval z %z% +1
    global z
    remote z %actor%
  end
  %send% %actor% %x%, %y%, %z%
elseif %cm% == вниз || %cm% == down
  if %z% < 0
    eval z %z% +1
    global z
    remote z %actor%
  else
    eval z %z% -1
    global z
    remote z %actor%
  end
  %send% %actor% %x%, %y%, %z%
else
  return 0
end
~
#167149
убиение всех на локации~
0 z 100
~
eval flag 0
while %flag% != 1
  foreach i %list%
    if %i% == %self%
      halt
    else
      mkill %i%
    end
  done
  if %self.all% != 0
    eval list %self.all%
    global list
  else
    eval flag 1
    global flag
  end
done
~
#167150
запуск убиения всех на выбранной локации~
1 c 1
гнев~
set room %arg.car%
if %world.room(%room%)% == 0
  %send% %actor% такой комнаты не существует
  halt
end
set room %world.room(%room%)%
eval rroom %actor.realroom%
eval list %room.all%
%actor.room(%room.vnum%)%
attach 167149 %actor%
exec 167149 %actor%
detach 167149 %actor%
%actor.room(%rroom%)%
~
$
$
