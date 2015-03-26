#37800
попал в провал~
2 g 100
~
wait 1s
wecho С криком Вы полетели вниз!
wait 5s
wteleport all 37821
wait 1s
wat 37821 wsend %actor.name% Вы упали вниз и сильно ушиблись!
if %actor.sex%==1
wat 37821 wechoaround %actor.name% упал вниз и сильно ушибся!
else
wat 37821 wechoaround %actor.name% упала вниз и сильно ушиблась!
end
~
#37801
рубим паутину~
2 c 100
рубить~
if !%arg.contains(паутин)%
  wecho Чаго?
  halt
end
wait 1s
wsend %actor% Размахнувшись, Вы разрезали паутину.
wechoaround %actor% Размахнувшись, %actor.name% разрезал%actor.g% паутину.
wdoor 37811 north flags a
wdoor 37811 north room 37812
wdoor 37812 south flags a
wdoor 37812 south room 37811
wait 1s
calcuid pautina 37801 obj
wait 1
wpurge %pautina%
wload obj 37804
detach 37801 %self.id%
~
#37802
резет зоны~
2 f 100
~
wdoor 37811 north purge
wdoor 37812 south purge
wdoor 37840 north purge 
wdoor 37840 south purge
wdoor 37840 east purge
wdoor 37841 north purge 
wdoor 37841 south purge
wdoor 37841 east purge
wdoor 37835 north purge
wdoor 37835 south purge
wdoor 37835 east purge
wdoor 37839 north purge
wdoor 37839 south purge
wdoor 37839 east purge
wdoor 37838 north purge
wdoor 37838 south purge
wdoor 37838 east purge
wdoor 37834 north purge
wdoor 37834 south purge
wdoor 37834 east purge
wdoor 37836 north purge
wdoor 37836 south purge
wdoor 37836 east purge
wdoor 37837 north purge
wdoor 37837 south purge
wdoor 37837 east purge
wdoor 37842 north purge
wdoor 37842 south purge
wdoor 37842 east purge
calcuid pautina 37801 obj
wait 1
wpurge %pautina%
wload obj 37801
detach 37801 %self.id%
attach 37801 %self.id%
~
#37803
40~
2 e 100
~
wait 1s
wdoor 37840 north flags a
wdoor 37840 north room 37839
wdoor 37840 south flags a
wdoor 37840 south room 37841
wdoor 37840 east flags a
wdoor 37840 east room 37838
~
#37804
41~
2 e 100
~
wait 1s
wdoor 37841 north flags a
wdoor 37841 north room 37839
wdoor 37841 south flags a
wdoor 37841 south room 37835
wdoor 37841 east flags a
wdoor 37841 east room 37838
~
#37805
35~
2 e 100
~
wait 1s
wdoor 37835 north flags a
wdoor 37835 north room 37840
wdoor 37835 south flags a
wdoor 37835 south room 37834
wdoor 37835 east flags a
wdoor 37835 east room 37839
~
#37806
39~
2 e 100
~
wait 1s
wdoor 37839 north flags a
wdoor 37839 north room 37836
wdoor 37839 south flags a
wdoor 37839 south room 37841
wdoor 37839 east flags a
wdoor 37839 east room 37838
~
#37807
38~
2 e 100
~
wait 1s
wdoor 37838 north flags a
wdoor 37838 north room 37836
wdoor 37838 south flags a
wdoor 37838 south room 37835
wdoor 37838 east flags a
wdoor 37838 east room 37834
~
#37808
34~
2 e 100
~
wait 1s
wdoor 37834 north flags a
wdoor 37834 north room 37841
wdoor 37834 south flags a
wdoor 37834 south room 37836
wdoor 37834 east flags a
wdoor 37834 east room 37839
~
#37809
36~
2 e 100
~
wait 1s
wdoor 37836 north flags a
wdoor 37836 north room 37841
wdoor 37836 south flags a
wdoor 37836 south room 37837
wdoor 37836 east flags a
wdoor 37836 east room 37835
~
#37810
37~
2 e 100
~
wait 1s
wdoor 37837 north flags a
wdoor 37837 north room 37839
wdoor 37837 south flags a
wdoor 37837 south room 37834
wdoor 37837 east flags a
wdoor 37837 east room 37842
~
#37811
42~
2 e 100
~
wait 1s
wdoor 37842 north flags a
wdoor 37842 north room 37836
wdoor 37842 south flags a
wdoor 37842 south room 37834
wdoor 37842 east flags a
wdoor 37842 east room 37843
~
#37812
свет.паук умер~
0 f 100
~
if (%world.curobjs(37802)% < 10) && (%random.10% <= 2)
   mload obj 37802
end
~
#37813
дух умер~
0 f 100
~
if (%world.curobjs(37803)% < 8) && (%random.100% <= 7)
   mload obj 37803
end
~
#37814
пучиха умерла~
0 f 100
~
if (%world.curobjs(37800)% < 2) && (%random.100% <= 4)
   mload obj 37800
end
~
#37815
призрак умер~
0 f 100
~
if (%world.curobjs(37805)% < 3) && (%random.100% <= 4)
   mload obj 37805
end
~
#37816
лоад доспеха~
0 f 100
~
if (%world.curobjs(37800)% < 1) && (%random.2% == 1)
mload obj 37800
end
~
$~
