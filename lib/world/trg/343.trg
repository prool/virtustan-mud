#34300
засада в тракте3  в руме 34312~
2 e 100
~
  wdamage %actor% 19
  wsend       %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись вам в живот!!!!
  wechoaround %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись живот %actor.rname%!!!!
  wdamage %actor% 19
  wload mob 34300
  wload mob 34300
  calcuid glavar 34301 mob
  attach 34302 %glavar.id%
  exec 34302 %glavar.id%
  calcuid zasada 34312 room
  detach 34300 %zasada.id%
~
#34301
репоп тригов в зоне~
2 f 100
~
if !(%random.35% == 1)
calcuid zasada 34312 room
detach 34300 %zasada.id%  
calcuid zasada 34322 room
detach 34303 %zasada.id%
   return 0
   halt
end
switch %random.2%
case 1
calcuid zasada 34312 room
attach 34300 %zasada.id%  
break
case 2
calcuid zasada 34322 room
attach 34303 %zasada.id%  
break
     default
break
done
end
calcuid glavar 34301 mob
attach 34302 %glavar.id%
~
#34302
атакует моб из засады~
0 z 100
~
  mkill %actor%
wait 2
  say Бей их! За мной, ребятки!!!
detach 34302 %self.id%
~
#34303
засада в тракте3  в руме 34322~
2 e 100
~
  wdamage %actor% 19
  wsend       %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись вам в живот!!!!
  wechoaround %actor% "Засада! Разбойники!" - подумали вы, когда две стрелы воткнулись живот %actor.rname%!!!!
  wdamage %actor% 19
  wload mob 34300
  wload mob 34300
wload mob 34300
wload mob 34300
wload mob 34301
  calcuid glavar 34301 mob
  attach 34302 %glavar.id%
  exec 34302 %glavar.id%
detach 34303 %self.id%
~
#34304
подошел к сторожу~
0 h 30
~
wait 1
if %actor.vnum% == 34632
вста
mshou Братья! Не посрамим земли Русской! Бей угров!!!
end
wait 1
if %actor.vnum% == 34631
вста
mshou Братья! Не посрамим земли Русской! Все на угров!!!
end
wait 1s
say Эта дорога из Киева в Галич.
say Самая спокойная и надежная дорога на Руси-матушке. Я ее охраняю.
~
#34305
подошел к кресту~
0 q 10
~
wait 3s
взд
~
$~
