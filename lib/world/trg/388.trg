#38800
������� � ���� 38823~
2 e 100
~
wait 1s
wecho _���� �� ������ ���-����� ����� � �� ����� ����� ��������� �����.
wload mob 38818
wload mob 38818
calcuid oxboga 38822 room
exec 38827 %oxboga.id%
calcuid orxboa 38851 room
exec 38828 %orxboa.id%
calcuid torxboa 38824 room
exec 38828 %torxboa.id%
calcuid fertrum 38823 room
detach 38800 %fertrum.id%
~
#38801
����� �������� � ����~
2 f 100
~
calcuid f1ertrum 38823 room
detach 38800 %f1ertrum.id%
calcuid ferstrum 38823 room
attach 38800 %ferstrum.id%
calcuid d1rakrom 38870 room
detach 38809 %d1rakrom.id% 
calcuid dra1k1rom 38852 room
detach 38809 %dra1k1rom.id% 
calcuid fe2rtrm 38802 room
detach 38802 %fe2rtrm.id%
calcuid fertrm 38802 room
attach 38802 %fertrm.id%
calcuid hello 38823 mob
attach 38812 %hello.id%
calcuid sqtollik 38802 obj
attach 38816 %sqtollik.id% 
calcuid tron 38840 room
attach 38810 %tron.id% 
calcuid kashey 38800 mob
attach 38817 %kashey.id% 
wdoor 38885 down purge
wdoor 38884 down purge
calcuid wfqerstr 38858 room
attach 38822 %wfqerstr.id%
calcuid fqeum 38820 room
attach 38823 %fqeum.id%
calcuid crypt 38885 room
detach 38820 %crypt.id%
attach 38820 %crypt.id%
calcuid crypt2 38884 room
detach 38820 %crypt2.id%
attach 38820 %crypt2.id%
wait 1
calcuid podval 38832 room
detach 38821 %podval.id%
attach 38821 %podval.id%
~
#38802
������� � ���� 38802~
2 e 100
~
wait 1s
wecho _���� �� ������ ���-����� ����� � �� ����� ����� ��������� �����.
wload mob 38818
wload mob 38818
calcuid orxrbo 38801 room
exec 38827 %orxrbo.id%
calcuid otxtbo 38800 room
exec 38828 %otxtbo.id%
calcuid ytxtby 38803 room
exec 38828 %ytxtby.id%
calcuid fertrm 38802 room
detach 38802 %fertrm.id%
~
#38803
� ��� � ������������� ��������~
0 k 50
~
switch %random.6%
  case 1
    mecho _������������ �������� ����� ��������� �� ���� �������������� �������.
    mload mob 38806
  break
  case 2
    mecho _������������ �������� ����������, � ��������� �� ���� ��������� �������.
    mload mob 38807
  break
  case 3
    mecho _������������ �������� � ��������� ��������� �� ���� �������-�����.
    mload mob 38808     
  break
  case 4
    mecho _������������ �������� ����������, � � ����� ���� ������ ������� �����.
    mload mob 38809
  break
  default
  break
done
end
~
#38804
��������� ���� � ���� 38809~
2 e 100
~
if %direction%==down
  wsend %actor% _�� ��������� ���� � ����������, ��� ����, ������� ������ �� ���. 
  wsend %actor% _���� ����� ���.
end
~
#38805
����� � ����~
2 c 0
����� ������ �����������~
if (%arg.contains(����)%)||(%arg.contains(�����)%)
  if (%actor.move%<40)
    wsend %actor% _� ��� �� ������ ��� �� ����� �������.
    return 0
  else
    wsend %actor% _�� ������� � ����� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����� ����.
    wait 1s
    wsend %actor% _����� ����������� ���� �� ��������� � ���������� �����.
    if (%actor.realroom%==38805)
      wteleport %actor% 38806 
      eval temp %actor.move(-40)%
    elseif (%actor.realroom%==38824)
      wteleport %actor% 38825
      eval temp %actor.move(-40)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% �������%actor.u% ����.
  end
end
end
~
#38806
����� � ���� �����~
2 c 0
����� ������ �����������~
if (%arg.contains(����)%)||(%arg.contains(�����)%)
  if (%actor.move%<40)
    wsend %actor% _� ��� �� ������ ��� �� ����� �������.
    return 0
  else
    wsend %actor% _�� ������� � ����� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����� ����.
    wait 1s
    wsend %actor% _����� ����������� ���� �� ��������� � ���������� �����.
    if (%actor.realroom%==38806)
      wteleport %actor% 38805 
      %actor.move(-40)%
    elseif (%actor.realroom%==38825)
      wteleport %actor% 38824
      %actor.move(-40)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% �������%actor.u% ����.
  end
end
~
#38807
������� � ��������� ����~
2 c 0
������� ����� ���������~
if (%arg.contains(���)%)||(%arg.contains(���)%)
  if (%actor.move%<100)
    wsend %actor% _���-�� ������������ ���, ��� � ��� �� ������ ��� �� ���� ����.
    wsend %actor% _�� ������� � �������� ���� � ��������� �� �������� ������ �������� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����.
    wait 1s
    wsend %actor% _����� �� ��������, ��� ����������� � ���� �� ������!!! �� ������!!!
    wteleport %actor% 38888 
    return 0
  else
    wsend %actor% _�� ������� � �������� ���� � ��������� �� �������� ������ �������� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����.
    wait 1s
    wsend %actor% _����� ����������� ���� �� ��������� �� ����� ����� � �����������.
    if (%actor.realroom%==38857)
      wteleport %actor% 38881 
      eval temp %actor.move(-100)%
    elseif (%actor.realroom%==38815)
      wteleport %actor% 38816
      eval temp %actor.move(-100)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% �������%actor.u% ���� �� ����, ������������ � ���������.
  end
end
end
end
~
#38808
������� � ��������� ���� �����~
2 c 0
������� ����� ���������~
if (%arg.contains(���)%)||(%arg.contains(���)%)
  wsend %actor% _�� ���������� ������� � ����, �� ������� ��������� � ������.
  wsend  %actor% _���������� ����, ��� ������ ����� ����������.
  wechoaround %actor% _%actor.name% �����%actor.q% � ����, �� ������ ����� ������� ������ ������.
  return 0
end
~
#38809
����� � ����~
2 c 0
����� ������ �����������~
if (%arg.contains(����)%)||(%arg.contains(������)%)
  if (%actor.move%<10)
    wsend %actor% _� ��� �� ������ ��� ��� �����.
    return 0
  else
    wsend %actor% _�� ������� � ��������� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����.
    wait 1s
    wsend %actor% _�� ��������� �� ���������� ������ �����.
    if (%actor.realroom%==38852)
      wteleport %actor% 38878 
      eval temp %actor.move(-10)%
    elseif (%actor.realroom%==38870)
      wteleport %actor% 38879
      eval temp %actor.move(-10)%
    endif
    wat %actor.realroom% wechoaround %actor% _%actor.name% �������%actor.u% ����.
  end
end
end
~
#38810
������� ����~
2 c 0
������� �������� �������~
if (%arg.contains(����)%)||(%arg.contains(������)%)
  if (%actor.move%<90)
    wsend %actor%  � ��� �� ������ ��� ��� �����.
    return 0
  else
    wsend %actor%  �� �������� ����.
    eval buffer %actor.move(-90)%
    wechoaround %actor%  %actor.name%, ����������, �������%actor.g% ����.
    *if %random.100% < 75
    wload obj 38810
    *else
    *wecho   ...�� ��� ��� ������ �� ���������.
    *end
    calcuid tron 38840 room
    detach 38810 %tron.id% 
  end
end
~
#38811
� ��� � ������~
0 k 35
~
switch %random.15%
  case 1
    msend %actor% �����, ������� ������ �����, ������ ��� � �����!
    mechoaround %actor% �����, ������� ������ �����, ��������� %actor.dname% � �����!
    mdamage %actor% 200
    ����
  break
  case 2
    msend %actor% �����, ������� ������ �����, ������ ��� � ����!
    mechoaround %actor% �����, ������� ������ �����, ��������� %actor.dname% � ����!
    mdamage %actor% 100
    �����
  break
  case 3
    msend %actor% �����, ������� ������ �����, ������ ��� � ����� � ����� � ���!
    mechoaround %actor% �����, ������� ������ �����, ��������� %actor.dname% � ����� � ����� � ���!
    %actor.position(6)%
    %actor.wait(2)%
    mdamage %actor% 100
    ��
  break
  case 4
    mecho ������� ���� ����� ����������� ������ �������� ���!!!
    foreach target %self.pc%
      msend %target% �������� ���, ��������� ������, ������ ��� � ��� �� ������!
      mechoaround %target%  �������� ���, ��������� ������, ������ %target.rname% � ��� �� ������!
      mdamage %target% 250
    done
done
~
#38812
������ ��������~
0 d 0
������������ ���������� ������� ������~
wait 1s
say �������, ���� �� ������..
wait 1s
���
calcuid helslo 38823 mob
attach 38813 %helslo.id%
calcuid hello 38823 mob
detach 38812 %hello.id%
end
~
#38813
������ �������� 2~
0 d 0
����� ������ ��� ������ ��� ���� ������~
if %exist.obj(38811)%
  wait 1s
  say ������� �������... �� ��� ��� �������� ���� ������.
  ��
  return 0
else
  wait 2s
  say ��, �� �� ���� �, ��� ������ ��� ���-�� �����.
  wait 1s
  ���
  wait 1s
  say ��, ���������� �! �� ����-�� �������...�-�-�.
  wait 1s
  ���
  wait 2s
  say �������� � ������ ������������-�-�-�. ���� � ������-�-�-�.
  wait 1s
  ���
  wait 1s
  say � ��� ������-�� �� ����-�-�-�..
  wait 1s
  ���
  wait 1s
  say � ���������� ���, ����� ����� ������ � ����� ��������, ��� �� ������.
  wait 3s
  ��� 
  wait 1s
  say ��, �������, ������� �� ���� ������, ����. �-�-�-�.
  wait 1s
  ���
  wait 1s
  say ��, ������������ ����, ������� ����...
  ��
  wait 1s
  say ����� ��� � �� ���� ���, ��� ���������...
  wait 1s
  ���
  wait 1s
  say ����� ���� ������� �� ���������... � �������� �� �� �����������...
  wait 1s
  wait 1s
  say �������� ���� �������, ��� ������� �� ����� ���� � ��������������, � ���������, ��� ��� ��������� ��������...
  wait 1s
  ���
  mload obj 38811
  ���� ����� %actor.name%
  say ���.. ������ �� �������... ����� �� ����� ���� ���������� � ���� � ��������.
  calcuid hello 38823 mob
  detach 38813 %hello.id%
end
~
#38814
������ �������� 3~
0 d 0
������ ������~
calcuid heqllo 38823 mob
detach 38814 %heqllo.id%
end
~
#38815
������������ �������� �������~
0 f 100
~
if (%world.curobjs(38818)% <1) && (%random.2% == 1)
  mload obj 38818
end
if (%world.curobjs(569)%==0) && (%random.6%==1)
  mload obj 569
end
mecho _�� ����� ������������ �������� ���������� ��������� ����������.
mload mob 38806
mload mob 38807
mload mob 38808
mload mob 38809
mload mob 38806
~
#38816
��� �� ������� ��������~
1 p 50
~
wait 1s
oecho _�� �������� �������� ������ ��� ������������ �������������.
oload mob 38824
detach 38816 %self.id%
~
#38817
� ��� � ������ ���� � ���� ������ 500 ��~
0 l 100
~
if %self.hitp% < 600
  mecho _����� ����������� ������� ������� ��������� �������.
  mload mob 38821
  calcuid kassshey 38800 mob
  attach 38818 %kassshey.id% 
  calcuid kashey 38800 mob
  detach 38817 %kashey.id% 
end
~
#38818
� ��� � ������ ���� � ���� ������ 100 ��~
0 l 100
~
if %self.hitp% < 150
  mecho _����� ����������� ��������� �������, ��� ���������� ��� ����������.
  calcuid drakon 38821 mob
  attach 38819 %drakon.id% 
  exec 38819 %drakon.id%
  calcuid kawshey 38800 mob
  detach 38818 %kawshey.id% 
end
~
#38819
������ ������ �����~
0 z 100
~
wait 1
if (%self.realroom%==38832)
  mecho ������ ������� ������� ������� ������ ��� � �����...
  mecho ������ ��������� ���������� ����� �� ����� � ��� �����.
  calcuid werdrakon 38800 mob
  mteleport %werdrakon% 38886
  calcuid drakon 38821 mob
  calcuid podval 38832 room
  detach 38821 %podval.id%
  mpurge %drakon%
end
~
#38820
� ������� � ����������~
2 c 0
�������� ���������� ������� �������� ������ �������� ������~
if !%arg.contains(�����)% && !%arg.contains(�����)%
  halt
end
if !%actor.haveobj(38811)%
  wsend %actor% � ��� ��� �����.
  halt
end
wsend %actor%  �� �������� ��������� ���������.
wechoaround %actor%  %actor.name% �������%actor.g% ��������� �����...
wait 1
wecho ���������� ���� �������� �������� ����.
if (%actor.realroom%==38885)
  wdoor 38885 down flags ab
  wdoor 38885 down room 38821  
elseif (%actor.realroom%==38884)
  wdoor 38884 down flags ab
  wdoor 38884 down room 38848  
end
detach 38820 %self.id%
~
#38821
��� ������ � �����~
2 e 90
~
wait 1
if (%world.curmobs(38800)% > 0)
  eval chanse %random.10%*(%actor.int%+%actor.intadd%)
  if %chanse% > 92
    halt
  end
  wsend %actor% �� ��������� ������� ����� ����� ������������!
  wechoaround %actor% %actor.name% ��������%actor.g% ������� ����� ����� ������������!
  %actor.position(6)%
  %actor.wait(3)%
end
~
#38822
������� ����� 1~
2 c 0
������� ������ �������� ���������~
if (%arg.contains(�����)%)||(%arg.contains(�������)%)
  wsend       %actor% _�� �������� �����.
  wechoaround %actor% _%actor.name%, ����������, �������%actor.g% �����.
  calcuid pqervi 38852 room
  exec 38825 %pqervi.id%
  calcuid j145skejvi 38828 mob
  wpurge %j145skejvi%
  calcuid sqervi 38819 room
  exec 38825 %sqervi.id%
  calcuid rqervi 38818 room
  exec 38825 %rqervi.id%
  calcuid fqerstrum 38858 room
  detach 38822 %fqerstrum.id%
end
~
#38823
������� ����� 2~
2 c 0
������� ������ �������� ���������~
if (%arg.contains(�����)%)||(%arg.contains(�������)%)
  wsend       %actor% _�� �������� �����.
  wechoaround %actor% _%actor.name%, ����������, �������%actor.g% �����.
  calcuid pqwervi 38870 room
  exec 38825 %pqwervi.id%
  calcuid j14skejvi 38827 mob
  wpurge %j14skejvi%
  
  calcuid tqervi 38869 room
  exec 38825 %tqervi.id%
  calcuid yqervi 38868 room
  exec 38825 %yqervi.id%
  calcuid fqeum 38820 room
  detach 38823 %fqeum.id%
end
~
#38824
�������� ��������~
0 z 100
~
mecho _������ ������ ������� ��� �������� ��������� ����.
mpurge %self.id%
~
#38825
������ ���� � ����~
2 z 100
~
wecho _������-�� �������� �������� �������� ���.
wecho _��� ������� ����� �� ���, ��������� ��� �� ����.
foreach victim %self.all%
  if !%victim%
    halt
  end
  wdamage %victim% 1000
done
~
#38826
����� �����~
0 f 100
~
if %world.curobjs(1254)% <1 &&  %world.curobjs(1291)% <1
  if %world.curobjs(1255)% <1
    if %random.100% < 3
      mload obj 1254
    end
  end
end
if (%world.curobjs(38819)% <1) && (%random.1000% <= 200)
  mload obj 38819
end
if (%world.curobjs(38821)% <2) && (%random.1000% <= 200)
  mload obj 38821
end
if (%world.curobjs(567)%==0) && (%random.1000% <= 160)
  mload obj 567
end
if (%world.curobjs(565)%==0) && (%random.1000% <= 160)
  mload obj 565
end
if (%world.curobjs(38822)% <2) && (%random.1000% <= 200)
  mload obj 38822
end
if (%world.curobjs(226)% <20) && (%random.1000% <= 200)
  mload obj 226
end
~
#38827
��������� ���� ��.��������~
2 z 100
~
wait 1s
wecho _���� �� ������ ���-����� ����� � �� ����� ����� ������� �������.
wload mob 38818
~
#38828
��������� ���� �������� ���������~
2 z 100
~
wait 1s
wecho _���� �� ������ ���-����� ����� � �� ����� ����� ��������� �����.
wload mob 38819
wload mob 38819
~
#38829
������ �������~
0 k 30
~
������
������ 
wait 1s
�����
~
#38830
����� �������~
0 f 100
~
if (%world.curobjs(559)%==0) && (%random.3%==1)
  mload obj 559
end
~
#38831
��� ����� � ���� ���� ���� � ����~
0 c 0
����� ������ ����������� ��������~
if (%arg.contains(����)%)||(%arg.contains(������)%)
  if (%actor.move%<10)
    wsend %actor% _� ��� �� ������ ��� ��� �����.
    return 0
  else
    wsend %actor% _�� ������� � ��������� ����.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����.
    wait 1s
    wsend %actor% _������ �������� ������ ������� ��� ������, ��� �������, � ������� ��������.
    wechoaround %actor% _%actor.name% �����%actor.q% � ����, �� �������� ������ ������ ���� ��������� %actor.vname% ��������.
    if (%actor.realroom%==38852)
      wteleport %actor% 38819 
    elseif (%actor.realroom%==38870)
      wteleport %actor% 38869
    endif
  end
end
end
~
#38832
����� ���.������ 38827~
0 f 100
~
calcuid drakrom 38870 room
attach 38809 %drakrom.id% 
~
#38833
����� ���.������ 38828~
0 f 100
~
calcuid dra1krom 38852 room
attach 38809 %dra1krom.id% 
~
#38897
new trigger~
0 fg 100
~
mload mob 38897
mload mob 38897
~
$~
