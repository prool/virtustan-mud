#35100
первых вход к Пуду~
0 r 100
~
if (!(%exist.mob(35110)%) && !(%exist.mob(3511)%) && !(%exist.mob(35112)%))
  halt
end
if !(%exist.mob(35103)%) 
  halt
end
улыб %actor.name%
msend %actor% - Здраве тебе, %actor.name%!
msend %actor% - Нечасто бывают у меня здесь гости.
msend %actor% - Могу ли я рассчитывать на твои услуги в важном деле?
attach 35103 %self.id%
detach 35100 %self.id%
~
#35101
портал из капища к Пуду~
2 z 0
~
wportal 35184 2
detach 35101 %self.id%
~
#35102
убит идол~
0 f 100
~
mload obj 35102
%echo% - \&WРазрушив идола вы почувствовали себя несколько чище от скверны язычников.\&n
if ((%world.curobss(553)% < 1) && (%random.100% < 10))
  *книга "звуковая волна"
  *mload obj 553
end
~
#35103
персонаж говорит да~
0 d 1
попытаюсь да согласен~
if !(%exist.mob(35103)%)
  halt
end
say - В нашем пристанище стали проявляться разные нехристи...
say - Я думаю что это проделки язычников, которые уже несколько раз
say - пытались выжить нас с этой благодатной земли, завоеванной 
say - нашими пращурами.
wait 1s
say - Недавно несколько язычников сумели даже пробраться
say - внутрь частокола и хотели запалить церковь!
say - Не иначе, кто-то из моих людей не устоял пред диавольскими
say - кознями и склонился к язычеству.
say - Попытйся разузнать что-нибудь об этом, да смотри - крови зря не проливай!
attach 35109 %self.id%
detach 35103 %self.id%
~
#35104
оплата квеста~
0 j 100
*~
wait 1s
if (%object.vnum%==35102) 
  wait 1
  mpurge %object%
  if (%actor.sex% == 1)
    msend %actor% - Поклон тебе , %actor.name% , от всех лесомык!
  elseif
    (%actor.sex% == 2)
    msend %actor% - Спасибо тебе, красавица, уважила лесомык.
    msend %actor% Пуд Волкорез ласково погладил Вас по волосам.
  else
    msend %actor% - Спасибо тебе, %actor.name% , помог ты лесомыкам своим делом.
  end     
  wechoaround %actor% Пуд Волкорез поблагодарил %actor.rname%.
  switch %actor.class%
    * лекарь
    case 0
      if (%world.curobjs(35108)% < 20) && (%random.10% <= 3)
        mload obj 35108
        дать поп.ряс %actor.name%
      else
        %self.gold(+3000)%
        дать 3000 кун %actor.name%
      end
    break
    * вор
    case 2
      if (%world.curobjs(35109)% < 10) && (%random.10% <= 5)
        mload obj 35109
        дать ржа.заточ %actor.name%
      elseif (%world.curobjs(35110)% < 7) && (%random.10% <= 2)
        mload obj 35110
        дать остр.заточ %actor.name%
      else
        %self.gold(+2000)%
        дать 2000 кун %actor.name%
      end
    break
    * богатырь
    case 3
      if (%world.curobjs(35104)% < 10) && (%random.10% <= 2)
        mload obj 35104
        дать был.булава %actor.name%
      else
        %self.gold(+2000)%
        дать 2000 кун %actor.name%
      end
    break
    * наемник
    if (%world.curobjs(35109)% < 10) && (%random.10% <= 5)
      mload obj 35109
      дать ржа.заточ %actor.name%
    elseif (%world.curobjs(35110)% < 7) && (%random.10% <= 2)
      mload obj 35110
      дать остр.заточ %actor.name%
    else
      %self.gold(+2000)%
      дать 2000 кун %actor.name%
    end
    case 4
    break
    * дружинник
    case 5
      if (%world.curobjs(35105)% < 15) && (%random.10% <= 1)
        mload obj 35105
        дать был.щит %actor.name%
      elseif (%world.curobjs(35106)% < 10) && (%random.10% <= 3)
        mload obj 35106
        дать кистень %actor.name%
      else
        %self.gold(+1500)%
        дать 1500 кун %actor.name%
      end
    break
    * витязь
    case 9
      if (%world.curobjs(35105)% < 15) && (%random.10% <= 1)
        mload obj 35105
        дать был.щит %actor.name%
      elseif (%world.curobjs(35106)% < 10) && (%random.10% <= 3)
        mload obj 35106
        дать кистень %actor.name%
      else
        %self.gold(+1500)%
        дать 1500 кун %actor.name%
      end
    break
    default
      %self.gold(+5000)%
      дать 15000 кун %actor.name%
    break
  done
end
~
#35105
помер волхв~
0 f 100
~
if ((%random.100% < 10 ) && (%world.curobjs(35112)% < 3))
  mload obj 35112
end
~
#35107
репоп зоны Пристанище лесомык~
2 f 100
~
eval num1 %random.100%
eval num2 %random.100%
eval num3 %random.100%
eval mobeq (%num1%+%num1%+%num1%)/3
if (%mobeq% > 66)
  calcuid ranger 35110 mob
elseif (%mobeq% > 33)
  calcuid ranger 35111 mob
else
  calcuid ranger 35112 mob
end
attach 35108 %ranger.id%
exec 35108 %ranger.id%
calcuid pud 35102 mob
detach 35100 %pud.id%
detach 35103 %pud.id%
detach 35104 %pud.id%
detach 35109 %pud.id%
detach 35110 %pud.id%
attach 35100 %pud.id%
~
#35108
лоад амулета~
0 z 0
~
mjunk all
mload obj 35111
detach 35108 %self.id%
~
#35109
волкорезу сдали амулет~
0 j 100
~
wait 1
if (%object.vnum% != 35111)
  say Зачем ты мне это принес?
  ворч
  give all %actor.name%
  halt
end
if ( %actor.vnum% != -1 )
  дум
  give амулет %actor.name%
  halt
end
if %exist.mob(35110)%
  set num1 1 1
end
if %exist.mob(35111)%
  set num2 1 1
end
if %exist.mob(35112)%
  set num3 1
end
eval sum %num1%+%num2%+%num3%
say Ага, ты наш%actor.q% отступника... Это хорошо.
if (%sum% < 2)
  хмур
  say Но что я говорил?!
  say Людей мне трогать не смей попусту, а ты?!
  say Половину селения истребили походу!
  say Вон с глаз моих!!!
  drop all
  detach 35109 %self.id%
end
wait 3
say Однако же, кто-то научил его и против веры христовой наставлял.
say Чую я в этом амулете волшбу сильную, нечестивую.
say Пожалуй, по слову моему он сумеет открыть дорогу к нечестивому капищу.
say Старики сказывают - если разрушить идола - недобрая сила навеки эти места покинет.
say Согласишься ли ты попытаться управиться с этим злом?
set questor %actor%
global questor
attach 35110 %self.id%
detach 35109 %self.id%
~
#35110
волкорез отсылает на капище~
0 d 1
да возьмусь пойду~
wait 2
emot поднял амулет перед глазами, всмотрелся в него и что-то прошептал
calcuid selfroom %self.realroom% room
exec 35111 %selfroom.id%
say Когда захочешь вернуться - просто брось его наземь.
give questitem351 .%questor.name%
attach 35104 %self.id%
detach 35110 %self.id%
~
#35111
портал от Пуда на капище~
2 z 0
~
wportal 35193 2
~
#35112
бросили амулет на капище~
1 h 100
~
wait 1
if ((%self.room% < 35191) || (%self.room% > 35196))
  halt
end
if (%exist.mob(35103)% || %exist.mob(35108)% || %exist.mob(35109)% || %exist.mob(35107)%)
  halt
end
%echo% Амулет вспыхнул багровым огнем и рассыпался в прах.
calcuid selfroom %self.room% room
attach 35101 %selfroom.id%
exec 35101 %selfroom.id%
wait 1
%purge% %self%
~
$~
