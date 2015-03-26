#6000
достаем сюикен~
1 c 1
достать~
if %arg.contains(сюикен)%
  if %world.curobjs(6018)% >= 500
    osend %actor% Сунув руку в пояс, вы обнаружили что сюикены закончелись!
    halt
  end
  oload obj 6018
  osend %actor% Вы достали сюикен из пояса
  oechoaround %actor%   %Actor.name% достал  %actor.g% сюикен из пояса.
  oforce %actor% взять сюикен
  oforce %actor% вооружиться сюикен
else
  osend %actor% Что вы хотите достать?
end
~
#6001
кольцо~
1 j 100
~
oforce амрина ор хочууу, младина хочууу!
done
~
#6002
хил~
1 c 1
хил~
oload obj 9926
oforce %actor.name% вз пуз
oforce %actor.name% осуш пуз
done
~
#6005
new trigger~
0 bg 100
~
г Ё, я Ёлко, &Gзеленая&n иголко!
end
~
#6099
труп эрвика~
1 g 100
~
if %actor.level% > 31
  halt
end
if %actor.name% != Эрвяк
  osend %actor% &MТруп Эрвика страшно щелкнул зубами и Вы отдернули руку&n
  oechoaround %actor% &MТруп Эрвика страшно защелкал зубами когда %actor.name% попытался его потревожить.&n
  otransform 6099
  return 0
  halt
end
~
$~
