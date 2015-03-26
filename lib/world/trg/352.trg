* BRusMUD trigger file v1.0
#35200
проход в локации Около реки~
2 e0 100
*~
wsend %actor% Проходя по болотистой местности вы слегка повредили ноги острой осокой растущей вдоль реки.
if %actor.sex%=2
   wechoaround %actor% Придя сюда %actor.name% выглядит слегка поврежденной.
end
if %actor.sex%=1
   wechoaround %actor% Придя сюда %actor.name% выглядит слегка поврежденным.
end
     eval    dam352  %actor.hitp%/7
wait 1s
wdamage %actor% %dam352%
*worlds dam352









~
#35201
поджечь костер~
2 c0 100
поджечь~
if !(%arg.contains(ветки)%)
   wsend %actor% Что поджигать то будем?
   halt
end
if (%world.curobjs(35202)%<1)
   wsend %actor% И где вы хворосту то раздобудете для костра?
   halt
end
wait 1
wpurge хворост
wload obj 35205
wsend %actor%  _Собрав хвороста вы разожгли небольшой костер.
if (%actor.sex%==2)
   wechoaround &CСобрав хвороста %actor.name% разжег небольшой костер.
else
   wechoaround &CСобрав хвороста %actor.name% разожгла небольшой костер.
end
wait 1s
wecho &RЯрко горит костер - ярко - только надолго ли...
wait %random.100%s
wecho &YВот и начинает костер потихоньку затухать...
wait %random.200%s
wecho &wНа месте костра видны угольки красного цвета... скоро и они погаснут...
wait %random.60%s
wecho &cВместо костра теперь виднеются только черные головешки, правда они еще дымятся
wpurge костер
wload obj 35201
wload obj 35201
wload obj 35201









~
#35202
головешка~
0 j0 100
*~
if ( %object.vnum% != 35201 )
  msend %actor% "Р-р-р-р-р-р-р... " - глухо зарычало чудовище.
  атаковать %actor.name%
  return 0
  halt
end
msend %actor% Схватив головешку чудовище подавилось...
msend %actor% И начало кашлять черным дымом...
mechoaround %actor% Схватив головешку чудовище подавилось, и начало кашлять черным дымом.
mload mob 35209
mpurge %self%









~
#35203
собрать ветки~
2 c0 100
собрать~
if !(%arg.contains(ветки)%)
wsend %actor% Что вы хотите собрать?
halt
end
   wsend %actor%  Вы начали собирать ветки деревьев для костра.
   wechoaround Наклонившись %actor.name% начал%actor.g% собирать хворост для костра.
wait 1s
wload obj 35202












~
#35204
триг на череп~
1 j0 100
*~
eval owner %self.carried_by%
if (%random.10%<=1)
dg_cast 'защ'  %owner.name%
end
if (%random.10%<=1)
dg_cast 'зату'  %owner.name%
end
if (%random.20%<=1)
dg_cast 'сила'  %owner.name%
end
if (%random.40%<=1)
dg_cast 'полет'  %owner.name%
end
if (%random.100% < 40)
otransform 35210
end





~
#35205
поп черепа~
0 f0 100
*~
if (%world.curobjs(35203)% < 5) && (%random.10% <= 2)
   mload obj 35203
end





















~
#35206
триг на хвост~
1 j0l0 100
*~
eval owner %self.worn_by%
if (%random.50%<=1)
dg_cast 'крит исц'  %owner.name%
end
if (%random.100% < 40)
otransform 35211
end


~
#35207
поп хвоста~
0 f0 100
*~
if (%world.curobjs(35204)% < 5) && (%random.100% <= 5)
   mload obj 35204
end

































~
#35208
триг на предупреждение~
0 q0 100
*~
mecho %Self.name% сказал Вам :
mecho \&RОсторожно путник...
mecho \&RДорога на север ведет в страшное болото названное еще нашими пращурами   
mecho \&RПрорва. Старики говорят что Прорва непроходима  для  простых  людей, надо 
mecho \&Rбыть очень смелым и храбрым, чтобы пройти до  центра Прорвы.  Каждая пядь
mecho \&Rболотистой местности таит в себе  опасность. Один  неверный шаг и ты ока-
mecho \&Rжешься  в смертельных  объятиях  трясины. Но  есть в ней тропинка отмеченная
mecho \&Rдеревьями с метками - иди по ней и не заплутай.




~
#35209
поп слизи~
0 f0 100
*~
if (%world.curobjs(35206)% < 5) && (%random.100% <= 10)
   mload obj 35206
end

































~
#35210
запинали фазана~
0 0 15
~
if %world.curobjs(35209)% < 5
mload obj 35209
end





~
$
$
