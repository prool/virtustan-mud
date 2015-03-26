#34000
Приветствие Паромщика~
0 r 100
1~
if %direction%==south
  смеять %actor.name%
  tell %actor.name% Ну что убедился?
  tell %actor.name% Только я смогу переправить тебя на другой берег реки.
  tell %actor.name% Но бесплатно я это делать не буду, только за 10 кун.
 else
wait 1s
  tell %actor.name% Здрав буде, %actor.iname%!
  tell %actor.name% Мост сейчас ремонтируют и ты не сможешь перейти по нему на другой берег.
  tell %actor.name% Разве что я помогу тебе.
  улыб
wait 1s
  tell %actor.name% За 10 кун я перевезу тебя на другой берег.
end
~
#34001
Переправа с востока на запад~
0 m 1
~
if %amount%>=10
if %amount%>10
  восторг %actor.name%
  tell %actor.name% Ух! Спасибо не каждый день встретишь таких щедрых!!
end
tell %actor.name% Щас быстро переправим! И глазом моргнуть не успеешь!!!
улыб %actor%
wait 1s
msend %actor% - Паромщик посадил вас в лодку, быстро отчалил и погреб к другому берегу.
mechoaround %actor% %actor.iname% залез%actor.q% в лодку к паромщику. И они поплыли к противоположному берегу
wait 2s
if %actor.realroom% == 34013
mteleport %actor% 34018 horse
else
mteleport %actor% 34013 horse
end
msend %actor% - Паромщик быстро пересек реку и высадил вас на противоположный берег.
mechoaround %actor% Паромщик причалил к берегу. Из лодки вылез%actor.q% %actor.iname%.
else
  tell %actor.name% 10 кун! Ты что считать не умеешь?
 give %amount% кун %actor.name%
end
~
#34002
Упасть в воду с недостроенного моста~
2 e 100
~
wait 1s
if (%time.minth% == 12 ) || (%time.minth% == 1 ) || (%time.minth% == 2 )
halt
end
if (%actor.realroom% != 34015)&&(%actor.realroom% != 34016)
  wsend %actor% Вы по уши в воде.
  halt
end
wsend %actor% - Поскользнувшись на бревне вы с криком полетели в воду... не зря мост ремонтируют :(
wechoaround %actor% - %actor.iname% поскользнул%actor.u% на бревне и с криком "М-А-М-А-А!!!" упал%actor.g% с моста, окатив вас водой.
if %actor.realroom% == 34015
  wteleport %actor% 34048
else
  wteleport %actor% 34049
end
~
#34003
рубим лес~
0 b 100
~
return 0
wait 1
if %self.fighting%
 halt
end
eval fchar %self.people%
while %fchar%
 if %fchar.vnum%==-1
  switch %random.18%
   case 1
    msend %fchar% Высоко подняв топор, дровосек рубанул по дереву в самое основание.
    break
   case 2
    msend %fchar% После сильного удара дровосека дерево чуть наклонилось..
    break
   case 3
    msend %fchar% От сильнейшего удара дровосека дерево повалилось на землю.
    mdamage %fchar.name% 15
    break
   case 4
    msend %fchar% Дровосек утер пот и продолжил работу.
    break
   default
    break
  done
 end
 eval pc %fchar.next_in_room%
 if %pc.id% && %pc.id%!=%fchar.id%
  makeuid fchar %pc.id%
 else
  unset fchar
 end
done
~
$~
