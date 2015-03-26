#30600
зашли в комнату к монаху-войну~
0 q 100
~
if %actor.religion% == 1
  wait 1       
  говор Добро пожаловать на земли Валаамского монастыря!
else
  wait 1       
  говор Язычникам здесь не место! Убирайся!
  mecho _Монах вытащил спрятанный под рясой меч.
  wait 5s       
  eval nesluh %random.pc%
  говор Зря ты меня не послушал%nesluh.u%! Теперь умри!
  воор меч
  kill %nesluh.name%
end
~
#30601
вошли к пьяному дьякону~
0 r 100
~
eval victim %random.pc%
msend       %victim% _Пьяный дьякон остановился на полпути к бочке, завидев Вас.
mechoaround %victim% _Пьяный дьякон остановился на полпути к бочке, завидев %victim.rname%. 
wait 1s
ик
say Ты! Ты зачем это здесь?
wait 1s
say Понял... До моего вина добраться хочешь!!!
рыч
mkill %victim%
~
#30602
зашли к хитрому келарю~
0 r 100
~
wait 1
говор Ага, еще один грабитель пожаловал!
mkill %actor% 
exec 30609 %world.room(30600)% 
mecho _Несколько крепких монахов прибежало сюда.
mload mob 30642
mload mob 30642
detach 30602 %self.id%
~
#30603
зашли к игумену~
0 q 100
~
if (%actor.religion% != 1)
  halt
end
wait 2s
вздох
emot мрачно задумался
wait 1s
mecho _Игумен тяжело вздохнул и произнес:
mecho - Все в руках Господних...
wait 2
mecho - Но вразуми, Господи, где взять силы и разум,
mecho - Дабы укротить корыстолюбцев, и замыслы коварные постичь?
mecho - Не для себя ведь прошу, Господи, для блага братии и веры христовой... 
emot зашептал молитву
~
#30604
выдача квеста у игумена (христиане)~
0 d 0
"В чем дело?" "Что случилось?" ~
if (%actor.religion% != 1)
  halt
end
if %questor306%
  halt
end
makeuid questor306 %actor.id%
global questor306
calcuid riznichiy 30621 mob
remote questor306 %riznichiy.id%
calcuid riznichiy2 30649 mob
*remote questor306 %world.mob(30649)%
remote questor306 %riznichiy2.id% 
*remote questor306 %world.mob(30621)%
* не пашет world. почему то :( бага имх
wait 2s 
say Странные дела творятся в обители... 
вздох
wait 2
mecho - То одного недостача, до в другом нехватка.
mecho - Но ранее лишь в малом так было.
wait 1
mecho - А недавно - промолвить страшно - похитили ковчег драгоценный со святыми мощами!
mecho - И как не убило на месте молнией святотатцев!
wait 1s
mecho - Как же найти вора? Ведь братия вся и крепка духом и вере верна...
mecho - А все ж видно затаился промеж нами аспид - поди угадай.
wait 3
mecho - Того, кто найдет сего змия ядовитого, я щедро вознагражу.
mecho - Ну а с чего начать... тут с архидиаконом посоветуйся, он муж разумный.
calcuid diak 30614 mob 
attach 30605 %diak.id% 
attach 30606 %diak.id% 
attach 30617 %self.id%  
~
#30605
у архидьякона (подсказка)~
0 q 100
~
if (%actor.religion% != 1)
  halt
end 
wait 1s
emot посмотрел на Вас
say Во Христе, да во здравии будь, %actor.name%!
say С чем пришел? Ищешь кого?
~
#30606
подсказка архидьякона~
0 d 0
"вора ищу" "ищу вора"~
if (%actor.religion% != 1)
  halt
end
if (%actor.vnum% != -1)
  halt
end 
wait 1s
say Хм... да.. слыхал я, неладное в обители творится.
wait 2
mecho - Но сказано, ежели в стаде пропадают овцы, может повинны в том волки...
mecho - А может и пастухи!
думать
wait 1s
say У ризничего вечно взгляд благостный да вороватый...
say Попробуй припугнуть его слегка - может и узнаешь чего
eval riznichiy %world.mob(30621)%   
detach 30607 %riznichiy%
attach 30610 %riznichiy%
~
#30607
пугают ризничего без повода~
0 c 1
пугать напугать испугать припугнуть пригрозить грозить~
if !%arg.contains(ризничий)% 
  msend %actor% _Вы начали грозно размахивать ручонками... жуть как страшно!
  mechoaround %actor% _%actor.name% погрозил%actor.q% всем пальцем - уууу я вас!
  halt
end
if (%actor.vnum% != -1)
  halt
end
wait 1s
хмур
say Это ты кому, мне что ли?!
say Да кто ты есть, чтоб мне грозить?!!
stand 
kill %actor.name%
exec 30608 %world.room(30600)%
mecho _Несколько крепких монахов прибежало сюда.
mload mob 30642
mload mob 30642
~
#30608
орет ризничий~
2 z 0
~
wzoneecho 30600 &Y "На помощь, братия!!!" - заорал ризничий. &n
~
#30609
орет келарь~
2 z 0
~
wzoneecho 30600 &Y "На помощь, братия!!!" - заорал хитрый келарь. &n
~
#30610
пугают ризничего~
0 c 1
пугать напугать испугать припугнуть пригрозить грозить~
if (%actor.religion% != 1)
  halt
end
if (%actor.vnum% != -1)
  halt
end
if !%arg.contains(ризничий)% 
  msend %actor% _Вы начали грозно размахивать ручонками... жуть как страшно!
  mechoaround %actor% _%actor.name% погрозил%actor.q% всем пальцем - уууу я вас!
  halt
end 
mforce %actor% кулак %self.name%
mforce %actor% say Ты, я гляжу, что-то знаешь?! А ну, говори!
wait 1s
пот
боя
wait 2
say Ладно, ладно... вы, я вижу, воители грозные...
say Ни в чем я не виноват, это всё
emot поперхнулся и замолк
wait 5
say Вот что я скажу... сходи в развалины старого монастыря, что на западе острова.
say Вот ключ от ворот. 
mload obj 30623
give ключ %actor.name%
wait 1
say Там ты найдешь того, кто на все твои вопросы ответит
wait 10s
emot воровато оглянувшись, выкинул в окно что-то похожее на камушек, с привязанной к нему запиской
*calcuid mainrobber 30643 mob
*detach 30616 %mainrobber.id%
*attach 30616 %mainrobber.id%
detach 30610 %self.id%
~
#30611
репоп зоны 306~
2 f 100
~
calcuid riznichiy 30621 mob
calcuid riznichiy2 30649 mob
calcuid igumen 30613 mob
calcuid diakon 30614 mob
calcuid razb1 30643 mob
rdelete questor306 %riznichiy.id%
rdelete questor306 %igumen.id%
rdelete kovcheg %igumen.id%
detach 30607 %riznichiy.id%
detach 30610 %riznichiy.id%
detach 30620 %riznichiy2.id%
attach 30620 %riznichiy2.id%
attach 30607 %riznichiy.id%
detach 30605 %diakon.id%
detach 30606 %diakon.id%
detach 30603 %igumen.id%
attach 30603 %igumen.id%
detach 30604 %igumen.id%
attach 30604 %igumen.id%
detach 30617 %igumen.id%
detach 30612 %razb1.id% 
attach 30612 %razb1.id% 
calcuid ushkuynik 30644 mob
wteleport %ushkuynik% 30688 
calcuid angel 30647 mob
wteleport %angel% 30688
calcuid ushkuynik 30645 mob
wteleport %ushkuynik% 30688
*calcuid ushkuynik 30646 mob
*wteleport %ushkuynik% 30688
wteleport %riznichiy% 30636       
wteleport %riznichiy2% 30688       
calcuid kelar 30622 mob
detach 30602 %kelar.id%
attach 30602 %kelar.id%
rdelete prikaz %igumen.id%
~
#30612
вошли к вожаку ушкуйников~
0 q 100
~
wait 1
say Эй, это ты тут суешь нос не в свои дела?
say Зря ты это, право слово... Меньше знаешь - крепче спишь!
say А кто слишком много знает, тот засыпает навсегда! 
detach 30612 %self.id%
~
#30613
подобрали ковчег~
1 g 100
~
if  (%actor.vnum% != -1)
  return 0
  halt                   
end
if (%actor.affect(освящение)% == 1)
  oecho Ковчег вспыхнул нестерпимым блеском, и медленно погас.
  detach 30613 %self.id%
  halt
end 
wait 1
set target %actor%
global target
if %exist.mob(30647)% 
  calcuid angel 30647 mob
  remote target %angel.id% 
  oteleport %angel% %self.room%
  exec 30614 %angel.id%
  %actor.wait(1)%
end
~
#30614
ангел атакует~
0 z 100
~
mecho _Внезапно все вокруг залило нестерпимое сияние, и с небес на землю снизошел ангел!
say Ты нечист%actor.g%, %target.name%! 
say Как ты посмел%actor.g% притронуться к святыне!
dg_cast 'длит оц' .%target.name%
dg_cast 'гнев бог'.%target.name%
kill .%target.name%
~
#30615
запинали ангела~
0 f 100
~
if %world.curobjs(3319)% < 1 && %world.curobjs(3320)% < 1 && %random.100% < 3
  mload obj 3319
end
return 0
mecho Небесный ангел грустно взглянул на Вас.
if ((%random.12% == 5) && (%world.curobjs(30625)% < 1)) 
  mload obj 30625
end
if %random.100% <= 15
  mload obj 595
end
~
#30616
убит главарь ушкуйников~
0 f 100
~
mload obj 30624  
mload obj 30626
if %world.curobjs(1234)% < 1
  if %world.curobjs(1235)% < 1
    if %random.10000% < 50
      mload obj 1234
    end
  end
end
~
#30617
дали предмет игумену~
0 j 100
~
if (%actor.vnum% != -1)
  halt
end
if (%actor.religion% != 1) 
  halt
end
wait 2 
switch %object.vnum%
  case 30624
    if (%actor.id% == %questor306.id%)
      say Ты нашел его!
      say Да.. сие достойно награды... но надо же сначала и вора отыскать...
      say Кто ж крадет тут у нас?
      eval kovcheg 1
      global kovcheg
    else 
      say Ты наш%actor.y% пропажу?
      say Я тебя о помощи не просил, но тем не менее награжу.
      mload obj 30628
      give кошель %actor.name%
      mpurge %object%
      detach 30617 %self.id%
    end 
    detach 30603 %self.id%
    mpurge %object%
  break
  case 30626
    if (%kovcheg% != 1)
      drop %object.name%
      halt
    end
    wait 1
    say Ну-ка что тут... 
    say Это же почерк ризничего!
    attach 30619 %world.mob(30621)% 
    wait 2
    say Ах он, прощелыга!
    say А ну-ка, приведи ко мне его!
    mpurge %object%
  break 
  default 
    wait 1s
    set objectname %object.name%
    if %objectname.contains(труп ризничего)%
      if (( %prikaz% != 1) || (%actor.id% != %questor306.id%))
        say Как посмел%actor.g% ты, %actor.name%, кровь проливать в священной обители?!
        say Стража!
        if !%actor.rentable%
          mkill %actor%
        else
          msend %actor% Тут же понабежавшие монахи утащили вас в темницу.
          mechoaround %actor% Откуда ни возьмись сбежалась толпа монахов и утащила %actor.rname%!
          mteleport %actor% 30654
        end
        detach 30617 %self.id%
        mpurge %object%
      else 
        wait 1
        say Вот кто, значит, был вором!
        say Говоришь, сам напал на тебя, заповеди и обеты презревши?!
        wait 2
        say Ах он, змий презренный!
        wait 1s
        say За труд свой заслужил%actor.g% ты награду.
        mpurge %object%
        exec 30618 %self.id% 
        halt
      end
    else
      say Хм.. Зачем мне это?
      give %object.name% %actor.name%
      halt
    end 
  done
~
#30618
награда у игумена~
0 z 0
~
eval object1 30628 + %random.3%
eval object2 30631 + %random.3%
eval object3 30634 + %random.3%
if %world.curobjs(%object1%)% < 3 && %random.4% == 1
  mload obj %object1%
elseif %world.curobjs(%object2%)% < 3 && %random.3% == 1
  mload obj %object2%
elseif %world.curobjs(%object3%)% < 3 && %random.3% == 1
  mload obj %object3%
elseif %world.curobjs(30638)% < 3 && %random.2% == 1
  mload obj 30638
else
  %self.gold(+7000)%
  say Вот твоя награда.
  дать 7000 кун %actor.name%
  halt
end
wait 1
say Вот твоя награда.
дать все %actor.name%
~
#30619
отвести вора к игумену~
0 d 0
"идем со мной" пошли пойдем~
*if (%actor.id% != %questor306.id%)
*halt
*end
*такое впечатление, что если у моба нет триггеров, то уничтожаются и переменные :(
*поэтому в таком виде на работает.. пришлось закомментить  
mecho _При виде Вас ризничий побледнел и затрясся 
wait 2s
say Куда я пойду? Да и зачем?
mforce %actor% say К игумену идем, ворюга!
wait 1s
say Что? К игумену.. ? Да что я там позабыл?
wait 4
emot начал рыдать и упрашивать вас отпустить его грешную душу на все четыре стороны
wait 2s
emot понял, что уговоры бесполезны, и повесил голову
wait 2
*follow %actor.name%
*mteleport %self% 30688
*calcuid riznichiy2 30649 mob
*mteleport %riznichiy2% 30636  
*exec 30621 %world.mob(30649)%
wait 3s
mechoaround %actor% злобно покосился на %actor.vname%
emot понял, что расправы не избежать и напал на Вас!        
dg cast 'оцеп' %actor.name%
kill %actor.name%
set prikaz 1  
global prikaz
calcuid igumen 30613 mob
remote prikaz %igumen.id%
detach 30619 %self.id%
~
#30620
ризничий атакует~
0 z 100
~
*wait 1
*if (%self.realroom% == 30619)
mechoaround %questor306% злобно покосился на %questor306.vname%
emot понял, что расправы не избежать и напал на Вас!        
dg_cast 'оцеп' %questor306.name%
kill %questor306.name%
eval prikaz 1  
global prikaz
remote prikaz %world.mob(30613)%
~
#30621
ризничий ╧2 следует за квестором~
0 n 0
~
follow %questor306.name%
~
#30622
запуск триггера 30620~
2 g 100
~
wait 1
if !(%actor.vnum% == 30649)
  halt
end
exec 30620 %actor.id%
~
#30623
запуск триггера 30620~
2 g 30
~
wait 1
if !(%actor.vnum% == 30649)
  halt
end
exec 30620 %actor.id%
~
#30624
встреча у лодочников~
0 q 80
~
wait 1
msend %actor% _Здорово живешь, %actor.name%!
if %self.realroom% == 30600
  msend %actor% _Всего лишь за полсотни кун доставлю я тебя на берег озера Ладожского.
else
  msend %actor% _За полсотни кун готов я отвести тебя на остров Валаам.
end 
~
#30625
оплата у лодочников~
0 m 1
~
wait 1
if %amount% < 50
  say Маловато будет!
  give %amount% кун %actor.name%
  halt
end
wait 1
msend %actor% _Вы уселись в лодку и лодочник принялся грести.
mechoaround %actor% _%actor.name% усел%actor.u% в лодку и лодочник принялся грести.
if (%self.realroom% == 30600)
  mteleport %actor% 30282
else
  mteleport %actor% 30600
end 
mechoaround %actor% _Кто-то приплыл сюда на лодке.
~
#30626
Игумен убит~
0 f 100
~
if (%world.curobjs(214)% < 50) && (%random.100% <= 20)
  mload obj 214
end
~
$~
