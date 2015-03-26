#37900
умерзеленыйзмей~
0 f 100
~
if %world.curobjs(37915)%==0
  mecho Змей умер, глаза огромного зверя остекленели, а один выпал из мертвой глазницы драгоценным камнем.
  mload obj 37915
end
~
#37901
вставляемзеленыйкамень~
2 c 100
вставить~
if !%arg.contains(камень)%
  wecho Чего?
  halt
end
calcuid kamen379 37915 obj
if %actor.eq(19)%==%kamen379%
  wecho Как?
  halt
end
if %actor.eq(17)%!=%kamen379%
  wecho Чего?
  halt
end
if (%arg.contains(камень)%) && (%actor.eq(17)%==%kamen379%)
  wsend %actor% Вы вставили драгоценный камень зеленого цвета в углубление в стене.
  if %actor.sex%==1
    wechoaround %actor% %actor.name% вставил драгоценный камень зеленого цвета в углубление в стене.
  else
    wechoaround %actor% %actor.name% вставила драгоценный камень зеленого цвета в углубление в стене.
  end
  wecho Что-то громко щелкнуло в стене, потом настала тишина.
  wait 4s
  wecho Вдруг стена разделилась пополам и разъехалась в стороны, образовав проход на юг.
  wdoor 37926 south room 37940
  wpurge %kamen379%
else
  wecho Куда вставить?
  halt
end
~
#37902
пуржезелкомнаты~
2 f 100
~
wdoor 37926 south purge
~
#37903
умеркрасныйзмей~
0 f 100
~
if %world.curobjs(37907)%==0
  mecho Змей умер, глаза огромного зверя остекленели, а один выпал из мертвой глазницы драгоценным камнем.
  mload obj 37907
end
~
#37904
вставляемкрасныйкамень~
2 c 100
вставить~
if !%arg.contains(камень)%
  wecho Чего?
  halt
end
calcuid kamen379 37907 obj
if %actor.eq(19)%==%kamen379%
  wecho Как?
  halt
end
if %actor.eq(17)%!=%kamen379%
  wecho Чего?
  halt
end
if (%arg.contains(камень)%) && (%actor.eq(17)%==%kamen379%)
  wsend %actor% Вы вставили драгоценный камень красного цвета в углубление в стене.
  if %actor.sex%==1
    wechoaround %actor% %actor.name% вставил драгоценный камень красного цвета в углубление в стене.
  else
    wechoaround %actor% %actor.name% вставила драгоценный камень красного цвета в углубление в стене.
  end
  wecho Что-то громко щелкнуло в стене, потом настала тишина.
  wait 4s
  wecho Вдруг стена разделилась пополам и разъехалась в стороны, образовав проход на восток.
  wdoor 37940 east room 37966
  wpurge %kamen379%
else
  wecho Куда вставить?
  halt
end
~
#37905
пуржекраснойкомнаты~
2 f 100
~
if %room.south%==east
  wecho Стена с треском сомкнулась, закрыв проход на восток.
end
wdoor 37940 east purge
~
#37906
умерчерныйзмей~
0 f 100
~
if %world.curobjs(37918)%==0
  mecho Змей умер, глаза огромного зверя остекленели, а один выпал из мертвой глазницы драгоценным камнем.
  mload obj 37918
end
~
#37907
вставляемчерныйкамень~
2 c 100
вставить~
if !%arg.contains(камень)%
  wecho Чего?
  halt
end
calcuid kamen379 37918 obj
if %actor.eq(19)%==%kamen379%
  wecho Как?
  halt
end
if %actor.eq(17)%!=%kamen379%
  wecho Чего?
  halt
end
if (%arg.contains(камень)%) && (%actor.eq(17)%==%kamen379%)
  wsend %actor% Вы вставили драгоценный камень черного цвета в углубление в стене.
  if %actor.sex%==1
    wechoaround %actor% %actor.name% вставил драгоценный камень черного цвета в углубление в стене.
  else
    wechoaround %actor% %actor.name% вставила драгоценный камень черного цвета в углубление в стене.
  end
  wecho Что-то громко щелкнуло в стене, потом настала тишина.
  wait 4s
  wecho Вдруг стена разделилась пополам и разъехалась в стороны, образовав проход на юг.
  wdoor 37966 south room 37949
  wpurge %kamen379%
else
  wecho Куда вставить?
  halt
end
~
#37908
пуржечернойкомнаты~
2 f 100
~
if %room.south%==south
  wecho Стена с треском сомкнулась, закрыв проход на юг.
end
wdoor 37966 south purge
~
#37909
умербелыйзмей~
0 f 100
~
if %world.curobjs(37919)%==0
  mecho Змей умер, глаза огромного зверя остекленели, а один выпал из мертвой глазницы драгоценным камнем.
  mload obj 37919
end
~
#37910
вставляембелыйкамень~
2 c 100
вставить~
if !%arg.contains(камень)%
  wecho Чего?
  halt
end
calcuid kamen379 37919 obj
if %actor.eq(19)%==%kamen379%
  wecho Как?
  halt
end
if %actor.eq(17)%!=%kamen379%
  wecho Чего?
  halt
end
if (%arg.contains(камень)%) && (%actor.eq(17)%==%kamen379%)
  wsend %actor% Вы вставили драгоценный камень белого цвета в углубление в стене.
  if %actor.sex%==1
    wechoaround %actor% %actor.name% вставил драгоценный камень белого цвета в углубление в стене.
  else
    wechoaround %actor% %actor.name% вставила драгоценный камень белого цвета в углубление в стене.
  end
  wecho Что-то громко щелкнуло в стене, потом настала тишина.
  wait 4s
  wecho Вдруг стена разделилась пополам и разъехалась в стороны, образовав проход на юг.
  wdoor 37949 south room 37913
  wpurge %kamen379%
else
  wecho Куда вставить?
  halt
end
~
#37911
пуржебелойкомнаты~
2 f 100
~
if %room.south%==south
  wecho Стена с треском сомкнулась, закрыв проход на юг.
end
wdoor 37949 south purge
~
#37912
встречастарогобогатыря~
0 r 100
~
wait 1s
чел
wait 1s
дум
if (%exist.mob(37925)% || %exist.mob(37925)%)
  mecho Привет!
  mecho Хочу предупредить тебя, здесь очень опасное место.
  mecho Здесь просто нет прохода от змеев, и притом, все разные.
  mecho Я например видел зеленых, красных, черных и белых. Еле сбежал.
  wait 1s
  mecho Но поговаривают, есть еще пещеры с особыми прозрачными змеями, самыми опасными.
  mecho Будь осторожен.
  wait 1s
  mecho Если ты все-таки их победишь, то может быть у меня найдется чем тебя наградить.
  wait 1
  halt
else
  mecho О, змеи всетаки были убиты. Здорово!
  if ((%world.curobjs(3344)% < 1) && (%world.curobjs(3345)% < 1) && (%random.1000% < 100))
    mload obj 3344
    mecho Вот, возьми. Мне она уже не потребуется.
    взд
    wait 1
    бр все
  end
end
wait 1
mpurge %self%
detach 37912 %self.id%
~
#37913
умербольшозмей~
0 f 100
~
mload obj 37925
switch %random.3%
  case 1
    if %world.curobjs(37924)%<2
      mload obj 37924
      halt
    end
  break
  case 2
    if %world.curobjs(37923)%<1
      mload obj 37923
      halt
    end
  break
  default
  break
done
if (%random.10%<4) && (%world.curobjs(575)%<1)
  mload obj 596
  halt
end
if (%random.10%<4) && (%world.curobjs(530)%<1)
  mload obj 530
end
~
$~
