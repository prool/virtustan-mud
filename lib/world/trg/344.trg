#34400
даем отроку бабло~
0 m 1
~
wait 1s
if %amount% < 2 then
  say За такую получку крутите воду сами, а меня увольте...
  пла
  wait 10s
  halt
end
wait 1
mecho _Отрок быстро начал крутить воду из колодца и наполнять фонтанчики.
calcuid rfont 34425 room
exec 34401 %rfont.id%
wait 10s
~
#34401
лоад фонтанчиков~
2 z 100
~
calcuid fontan 34400 obj
wpurge %fontan%
wload 34400 obj
~
#34402
входиш к отроку~
0 q 10
~
wait 1s
if %actor.sex% == 2
  wait 1s
  say  Всего за пару кун я наполню фонтанчики водой, о прекрасная госпожа.
else
  wait 1s
  say  Всего за пару кун я наполню фонтанчики водой, о славный путешественник.
end
~
#34403
входим к купцу-иноземцу~
0 q 100
~
wait 1
say Здрафстфуйте, голупчик!
say Мне надо попасть в Киеф - плачу щедро!
say Помошеш? Ес?
calcuid khelow 34421 mob
attach 34404 %khelow.id%
detach 34403 %khelow.id%
~
#34404
гриш помогу купцу~
0 d 0
помогу следуй готов~
wait 1s
say О ес! Пошли пыстее!
след %actor.name%
calcuid khelow 34421 mob
attach 34407 %khelow.id%
detach 34404 %khelow.id%
~
#34405
репоп зоны~
2 f 100
~
if %exist.mob(34421)%
  calcuid khelow 34421 mob
  detach 34404 %khelow.id%
  detach 34403 %khelow.id%
  attach 34403 %khelow.id%
end
~
#34406
купец грит сулчайно~
0 b 10
~
switch %random.13%
  case 1
    say Где я? Как интэрэсно....
    mecho Иноземный купец оглянулся по сторонам.
  break
  case 2
    say  Попыстрее бы в Киеф. Там польшой дело...
    взд
  break
  case 3
    mecho Иноземный купец старательно ощупал кошель, вшитый в одежду.
    mecho Иноземный купец облегченно вздохнул.
  break
  default
  break
done
end
~
#34407
гриш купцу привел~
0 c 0
привел все готово~
wait 1
if (%actor.realroom%==49987)
  say Польшое спасибо!
  msend %actor% Иноземный купец незаметно дал Вам 2000 кун.
  %actor.gold(+2000)%  
  пока
  mecho Иноземный купец ушел на постой.
  calcuid a7mfor 34421 mob
  mpurge  %a7mfor%
end
~
#34408
междугородный портал~
0 m 1
~
wait 1
emot пересчитал%self.g% монеты
eval target 0
switch %amount%
  * Русса (временно Великий Новгород)
  case 5200
    eval target 60016
  break
  * Любеч
  case 3400
    eval target 69073
  break
  default
    дум
    say И чего же ты за эти деньги хочешь?
    give %amount% кун .%actor.name%
    halt
  done
  кив
  emot сделал%self.g% несколько странных жестов
  mechoaround %actor% %actor.name% исчез%actor.q% в клубах дыма.
  msend %actor% У вас закружилась голова, и на миг вы потеряли сознание...
  msend %actor% 
  mteleport %actor% %target% horse
  mechoaround %actor% %actor.name% появил%actor.u% здесь в клубах дыма.
~
$~
