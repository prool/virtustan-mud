#167000
������ ����~
2 b 100
~
%echo% ���� ���������� ���
eval gopa %self.all%
foreach i %gopa%
  %damage% %i% 169
done

~
#167001
����������~
0 f 100
~
%echo% ��������� ���������, ����������� �� ������ ����.
if %actor.sex% == 1
  %send% %actor% �� �� ������ �������, ��� ������ ��- �����������!
elseif %actor.sex% == 2
  %send% %actor% �� �� ������ ��������, ��� ������ �� �����������
elseif %actor.sex% == 0
  %send% %actor% �� �� ������ ��������, ��� ������ �� �����������
elseif %actor.sex% == 3
  %send% %actor% �� �� ������ ��������, ��� ������ �� �����������
end
%echo% ������ �������� �� �����, ��� ���� ���������� � �������� ����� �����
%load% mob 167001
��� �� �����������, ������ ������!
dg_cast '���� ��' %actor%
dg_cast '���� ��' %actor%
%force% %actor% ��� �������� ����, ��������� �����, � �ӣ ������� � ����� �� ����� ���������
%send% %actor% ��ۣ� � ޣ���, �����!
%send% %actor% �������� ��� ����-�� ���������� ����������.
%echoaround% %actor% %actor.name% �������� ��������
%teleport% %actor% 167000
~
#167002
������������ ������~
1 c 1
������~
eval arg1 %arg.car%
eval victim %arg1%
eval dmg %arg.words(2)%
%send% %actor% �� ������� � %victim.vname% ������� �� %dmg% �����
calcuid rroom %victim.realroom% room
attach 167014 %rroom%
exec 167014 %rroom%
detach 167014 %rroom%
~
#167003
�������� �� ��������~
1 h 100
~
%echo% � ������ ��������� ��������� ���������, �� �������� �������  �������.
%send% %actor% ��������ģ���� ������� ����������� � �����, ������� ������, � ������� ������������ �������� ��� � ����� ������������
%echoaround% %actor% ��������ģ���� ������� ����������� � �����, ������� ������, � ������� ������������ �������� %actor.vname% � ������ ����� ��������
%teleport% %actor% 166022
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 166022
done
eval gopa %actor.group%
%echoaround% %gopa% � ����������� ���� �������� ������, �� �������� ���������:
foreach i %gopa%
  %echoaround% %i% %i.name%
  %send% %i% �������� ��� � ��������� ������ ������ ��������
done
%echo% ����� ����� ������ ��������, � ����������� �� ��� �������� ������� ��������� ������� � �����
~
#167004
��� �����~
1 c 2
���������~
if !%arg.contains(�����)%
  %send% %actor% � �������, ��� �� ultra_poglotitel_9000, �� ��������� ���� ���-��, � �� ������ ���
  halt
end
%send% %actor% �� ���� ����� �������� ��������� ����� �����, � ��� ��� ����� ����������, �� ����� �� ����� �������� ���������� ���. ����� ��� ���� ����������� �� ��������� �������, ������� ��������� �������� � �������
%echoaround% %actor% %actor.name% ����� ��������%actor.q% ����� �����, �� ����� �� �ţ �������� ���������� ���. ���� ����� ��� ���� ����������� �� ��������� �������, ������� ��������� ��������
%load% mob 167004
%send% %actor% ������ ����� ���������? �� ��� ��������! ������ �������� ����!
%purge% %self%
~
#167005
��������� ����� ����~
1 c 1
����~
eval arg1 %arg.car%
eval victim %arg1.id%
eval arg2 %arg.words(2)%
%send% %actor% �� ��������� ���� %victim.dname% � %arg2% ������
foreach i %victim.pc%
  if %i% == %victim%
    %send% %victim% ��� ����������, ��� ���� ���� ���������� � ������� ��� �� ���� ������, �� ����� ������ �������
    %victim.wait(%arg2%)%
  else
    %send% %i% %victim.name% �������� �����%victim.q% �� �����, ������ ������ ��� � ����� ���������
  done
end
~
#167006
����� �����~
1 c 2
���������~
if !%arg.contains(������)%
  %send% %actor% ����� �����, ������� ������!!!111 � ��� ��������� �� �����, ���������?
  halt
end
%send% %actor% �������� ������, �� ������������� ����������� ���� � ������� � �������� �������� ������ ���� ��������
%echoaround% %actor% ���������� ������� %actor.name% �������� %actor.q% ������ �����
dg_affect %actor% ��������� ������� 3 1440 0
%purge% %self%
~
#167007
��������� ���������� ���� �� ������� ����� ��������������� ~
1 h 100
~
%send% %actor% ������� ����������, ��������� ���� ����� ���
eval gopa %actor.all%
foreach i %gopa%
  %echo% ����������� %i.name%
  if %i.dex% > 50 || %i.clan% == �� || %i% == %actor%
    %send% %i% ��� ������
  else
    eval tmp %i.hitp% / 100
    eval tmp2 %tmp%*(%random.21% + 9)
    %echo% ������ ����  %i.hitp%  ����� %tmp2%
    %echo% ���� ������ %i.name% %tmp2%
    %damage% %i% %tmp2%
    %send% %i% ���� ������
  end
done
~
#167008
����� ����~
1 c 2
�������~
if !%arg.contains(���������)%
  %send% %actor% ��ɣ� ��ɣ�. � ���� �������� �� �����, ���?
  halt
end
if %actor.attackers% != 0
  %send% %actor% ������ �� � ���
else
  %send% %actor% ��  ���������� ��������� ����� "����� ����" ���� ��������!
  %echoaround% %actor% %actor.name% ������%actor.g% ��������� ����� "����� ����" ���� ��������!
  %load% mob 166023
end

~
#167009
����� �� ���������~
0 d 100
�����~
if %actor.attackers% != 0
  %send% %actor% ������ �� � ���
else
  ��� ����� ������, �� ��� �
  %echo% �������� ��� ���������� ����� � �������� �������� ������ ����������.
  %echo% ��� ������������  ��������� �����, �� ����� �� �������� � ���������� � �����
  wait 3
  %portal% 166022 2
  ��� ������� ����������� ����� ���� ���, ������� ���������
  %echo% ��������� ��� ����� �������� �������� ������� � �������
  %send% %actor% �����-�� ��������� ������� ��� ����������, ��� ��� �������� ������� � ��ϣ ����������
  %purge% %self%
end
~
#167010
���̣� �� �������������~
2 z 100
~
%portal% 166022 2
~
#167011
������~
1 c 1
������~
%send% %actor% �� ��� ���� ��� �����������, �
if %arg% < -1
  %send% %actor% ������ �� ���������
else
  %echoaround% %actor% %actor.name% �����%actor.q%
  %teleport% %actor% %arg%
  %echoaround% %actor% %actor.name% ������%actor.u% �� ��������
end

~
#167012
����������~
0 d 1
��������~
if %actor.attackers% != 0
  %send% %actor% ������ �� � ���
else
  %load% obj 166023
  ���� ���������� %actor.name%
end
~
#167013
����� ����~
0 c 2
��������~
if !%arg.contains(���������)%
  %send% %actor% �� �������� ������� � ���� chaos. ��, �� ��! ������ �������� ���� ���� ����� ����������, ���� ����-��, ��� �������?
  halt
end
%send% %actor% ��������� ������ �������, ��� �������� �������
%send% %actor% �����-�� ��������� ������� ��� ����������, ��� ��� �������� ������� � ��ϣ ����������
%echoaround% %actor% ���-�������� �������� ������� � �������
%purge% %self%
~
#167014
���� �������~
2 z 100
~
foreach i %victim.all%
  if %i% == %victim.id%
    %send% %victim% ��� ����� ������� ���������� �������� �������� ����, �� ������� %actor.name% ������ � ��� ��� ����� �������� ������� �������
    %damage% %victim% %dmg%
  else
    %send% %i% ��� ����� ������� ���������� �������� �������� ����, �� ������� %actor.name% ������ � %victim.vname% ������� �������
  done
end

~
#167015
����� �����~
1 h 100
~
wait 1
%send% %actor% ����� ����������� �� ������ �������, �������� �� ���� ����, ������� ������������ � ����� ������
%echoaround% %actor% %actor.name% ������%actor.q% �����, �� ������� ������ ����
eval gopa %actor.all%
foreach i %gopa%
  if %i% == %actor% || %i.leader% == %actor% || %i.name% == ������
    %send% %i% ���� ������� ��� ��������
    %echoaround% %actor% ���� ������� %actor.vname% ��������
  elseif %i.affected_by(������_�����) == 1
    %send% %i% ���� �������� ��� �� �����
  else
    eval tmp %random.1000%
    %send% %i% ���� �������� ���, �������� �����: %tmp%
    %echoaround% %i% ���� �������� %i.rname%, �������� �����: %tmp%
    %damage% %i% %tmp%
  end
done
%purge% %self%

~
#167016
���� ����~
0 k 100
~
eval tmp %self.hitp%*100/%self.maxhitp%
%echo% ������� ������� �����: %tmp% %
if %tmp% < 10 && %tmp1% != 1
  eval tmp1 1
  global tmp1
  %echo% ��� ����� �����
  %portal% 167011 20
  %teleport% %self% 167010
  world.zreset(1670)
end
if %tmp% < 25 && %tmp2% != 1
  eval tmp2 1
  global tmp2
  ��� � ��� �� ������� �� ���, ���� ��� ������?
  dg_cast '��������' %self%
  dg_affect %self% ���� ������_����� 1 6 1
end
if %tmp% < 50 && %tmp3% != 1
  eval tmp3 1
  global tmp3
  ��� ������ ��������?
  ���
  %load% obj 167025
  %load% obj 167025
  %load% obj 167025
  ������� ���.����
end
if %tmp% < 75 && %tmp4% != 1
  eval tmp4 1
  global tmp4
  ��� ��� ��- ������ ����, ������ ������� �� ����������� ��� ����� ������ � ���ң��, ��� ������� ������ ��������
  dg_cast '�������� ����������' %self.fighting%
end
~
#167017
������� �� ����� 1~
2 d 1
������~
%send% %actor% ����� ����� ����, ��� �� ������� ��� �����, ��� �������� �� �������� ����������� �������, � ����� ��������� �� ���� ��� ������ � ������ �����
%echoaround% %actor% ����� �� ����� ����, ��� %actor.name% ��������%actor.q% ��� �����, ��� �������� �� �������� ����������� �������, � ����� ��������� �� ��������� ���������� � ������ �����.
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167012
done
end
~
#167018
����� ���������~
1 i 100
~
eval gopa %actor.all%
foreach i %gopa%
  if %i% == %actor%
    %send% %actor% ��, �������������, ������� ������� � ������ ��������� � %victim.vname%
  elseif %i% == %victim%
    %send% %victim% �������������, %actor.name% ������%actor.g% � ��� ������� � �����-�� ������, � ��� ��������� ����� � ���, ���������� ������� �������
  else
    %send% %i% �������������, %actor.name% ������%actor.g% � %victim.vname% ������� � �����-�� ������
  end
done
dg_cast '����' %victim%
dg_cast '���� ����' %victim%
dg_cast '���� ������' %victim%
dg_cast '���� ����' %victim%
dg_cast '��' %victim%
%damage% %victim% 100
return 0
%purge% %self%

~
#167019
�������� �� �����~
2 d 1
������~
if %actor.haveobj(167033)% == 0
  %send% %actor% ������ ������ ����, � �� ��� ������� �� ���������
  halt
end
%echo% ����� ��� ����-�� ���������� ��������, � ����� ��������� �� ���� ��� �� �����
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167013
done

~
#167020
���̣� �� �������������~
2 z 100
~
%portal% 167011 20
~
#167021
������������ ������~
0 d 1
������ ����������� ������������ ������������ ������� ���~
��� �� ������������, ���� ������. ���������� ��, ��� � ��� ��������, ��� ����� ����� ����.
%echo% ������ ���������, ��������� �� ������� � �������, �����, � �� ������ , ��� ������ � ���� ������ �� ������������ ��������.
%echo% �� ����� �ӣ ��� ������������, � �� ���������, ������ ������� ������ �������
��� ����� � �� ��� ���� ������� ����������, �������� ������ �����������.
��� ����������, ������� ���-�� ��� �����, ������� ģ�����- � ��������� �����. ��� ��������! ��� �� ��� ���: ģ�����- � �����.
��� ���� ��� ����� ���� ������ ������, ��� � �� ������, �� ����� ����, � ��� �����.
��� �� ��� ����� ����� ������- �����. ��-��������� ���, �� ��-���������.!
��� ��� ��� ����������, ��� ����� �����������: ����� �� ��������� ţ � ޣ� ������, � ��� ���� ���� �� 3 ������ �������, �� �������� �������, ���� ��� �� �������. ��� �� ��� ���, �? ����� ��� �������, ������� ���������!
��� � ����� ������ ���� ��������, ���� � ���� �������������� �����-��, ������� ��� �����, ��� � � ����� �� ��� ��� �� �����, ����� �� ��� ��������?
attach 167022 %self%
detach 167021 %self%
~
#167022
������� �������~
0 d 1
�� ������ �������~
����
��� ��� ��� �� � ���� �������:
��� ��� �� ��� ���� ����������, ����� ���, ��� ���� �����, � �ݣ �������� �� ����, �� ���� �� ����. ������ �� ���� ��� ���������� �����, ��� �������� ��������.
��� � �������� ��� ���� ������ 3 �� ����� 5, � ����� �ݣ ������� ��� ������ �� ���������:
��� ��, ���, ��� ������ ���� ������ �������, � �� �� ���������, ��� ������, ��� ���� ���������. � ������ �� ��� ������ ��� ���� 8 ������� ����.
��� � ������� ���� �� ��� �������� ���������: � ������� �� ��� ������ ���������� �������� � �ۣ�, ��� ������.
wait 15s
attach 167023 %self%
����
������� �������???
%echo% ������ �� ����� ������� �������� ������� � ����� ����
%damage% %self% 20
wait 10s
������� �������������!!!
%echo% ������ ��������� ������ ������� � �����, �� ������� ��� ������ ���������� �������� �������
%damage% %self% 20
wait 10s
������� �������������, �����, ������� ���?
%echo% ��� �� ��������������, � �� ��� ��� ������� ��������� ���� ����
%damage% %self% 20
wait 10s
���� �� �������, ����, ������� ���?
%echo% ��� ���� ���� �� �����, ��� ��������� ��ϣ �������������� �������
%damage% %self% 20
wait 10s
%echo% �� � ����� ��������� � �����, ��� ������ ������� �� ������� ����, �� ����� � ���� ����� ��������� ������ �������, �� �������, ���������� � � ��������� ��� ������ ������� � �����, ���������� ���� ����� �� �����
%damage% %self% 10000

~
#167023
����� �� ������� ������~
0 d 1
���������� 13~
%echo% ������� ���, ��� �������� �����, ��� ���� �����������, �� ����������:
%echo% ������� ���, �������, �������!!! �� ���� �� �������������, ��� �������� �������, ������ � ����� ����� ���� �� �����. �������� ���, ����� ������ ������.
detach 167022 %self%
%load% obj 167033
���� ����� %actor.name%
attach 167021 %self%
detach 167023 %self%

~
#167024
���� ��� �����~
2 c 1
���������~
calcuid tmpmob 167013 mob
eval tmparg %arg%
if !%arg%
  %send% %actor% �� � ���� �� ������, ���� �������� ���������
else
  attach 167025 %tmpmob%
  run 167025 %tmpmob%
end
~
#167025
���������� ������� �� �����~
0 z 100
~
%send% %actor% �� ��������� ����� ������� %tmparg%
%tmparg%

~
#167026
������� � ������ 2~
2 e 100
~
wait 1
%echo% � ��� ������������� ����� ������
%echo% "�� ��������� �����. �� ��������� ������� ����� � �������� ��� ���������� ���������� �������, �� �������� �������� ������ ����.
%echo% ����, ��� ������� �������� � ��� � ���������� �� ���, ������ ���� ������������ ��������������, �� ����� ������ ���� ��������".
foreach i %self.pc%
  %damage% %i% 10000
done
~
#167027
�������� ������� ����� ������� (������������������� �������)~
1 c 4
��������~
if !%arg.contains(�������)%
  %send% %actor% �� � �� ��������, ���������, �� � ���������� ������� ������ ���� ����� ������, �������� ������
  halt
end
%send% %actor% �� �������� �������� �� ��������, ��������� ����, � ����� �� �������:
%echoaround% %actor% %actor.name% �������%actor.q% ������� �� ��������
%actor.room(167014)%
%force% %actor% ��
%actor.room(167013)%
~
#167028
������� �� ����� 3~
2 d 1
������~
if %actor.haveobj(167034)% == 0
  %send% %actor% ������ ������ ����, � �� ��� ������� �� ���������
  halt
end
%send% %actor% ��� ������ �� ��� �������, �� ������������� �������� ����� �� ������� �����, � ���� �������� �����, �������������� �� �����, ���������� ������ ��� ���� ��� ������ ������.
%echoaround% %actor% ��� ������ %actor.name% ������%actor.q% ���, �� ������������� �������� ����� �� ������� �����, � ���� �������� �������������� ����� �� �����, ���������� ������ ��� ����� ��� ������ ������
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167015
done

~
#167029
������� 1 �� ������ 3~
2 g 100
~
wait 1
%echo% ����� ���������� ���� �������, � �� �������� � ��� �� ���������
eval gopa %actor.pc%
foreach i %gopa%
  %i.move(1)%
done
eval tmp %random.2%
if %tmp% == 1
  foreach i %gopa%
    %damage% %i% 1000
    %send% %i% ����� ���������� ���� ��������� ����, �� ���������� ���� ����������� ����
  end
done
~
#167030
������ ������� � ������ 3~
2 g 100
~
wait 1
%echo% ����� ���������� ���� �������, � �� �������� � ��� �� ���������
eval gopa %actor.pc%
foreach i %gopa%
  %i.move(1)%
done
eval tmp %random.3%
if %tmp% == 1
  foreach i %gopa%
    %damage% %i% 1000
    %send% %i% ����� ���������� ���� ��������� ����, �� ���������� ���� ����������� ����
  end
done
calcuid tmproom 167019 room
attach 167031 %tmproom%
~
#167031
�����~
2 c 100
�������~
if !%arg.contains(�����)%
  %send% %actor% ���� ��� ģ������ �� �������, ���������� ��� ���
  halt
end
%send% %actor% �� ģ����� �����, � �������� �����-�� ���ޣ�. ������ ������ �� ���������, ���������. ����� ����� ���������� � ������� ��������?
wat 167015 %load% obj 167036
wat 167015 %echo% �������� ���ޣ�, � �� ��������� ����� �� ����� ���-�� �����
wat 167015 %echo% ����� ������: ������ �� ����� %actor.vname%, ��%actor.g% ��� �� ���Σ���.
wat 167015 %echo% ����� ���������� ���������, ��������� ����� ����� � ������
detach 167031 %self%

~
#167032
������������~
2 c 1
������������~
%send% %actor% ������������ ������������, �� ����� �� ���� ��� �� ������ � ������ ��������� ������� ���� ����� �� ��� �� ���
%echo% "�� ��������� �����. �� ��������� ������� ����� � �������� ��� ���������� ���������� �������, �� �������� �������� ������ ����.
%echo% ����, ��� ������� �������� � ��� � ���������� �� ���, ������ ���� ������������ ��������������, �� ����� ������ ���� ��������".
%damage% %actor% 1999999999
~
#167033
������� �� ����� 4~
2 d 1
������~
if %actor.haveobj(167036)% == 0
  %send% %actor% ������ ������ ����, � �� ��� ������� �� ���������
  halt
end
%echo% ���� ��������� ������� � �����, ����� � �����. �� ��� � ��� ������������ � �� ������ ������ � ������ �����.
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167020
done

~
#167034
���� ����� ����� �������~
0 f 100
~
%echo% �������� ������ � ������ �����, ��������� ������ ����� ���������, � ������� ���-�� �����
%load% obj 167035
%load% obj 167037
calcuid tmpobj 167035 obj
calcuid tmpuid 167037 obj
%tmpobj.put(%tmpuid%)%

~
#167035
�������� �� ������~
2 d 1
������~
if %actor.haveobj(167035)% == 0
  %send% %actor% ������ ������ ����, � �� ��� ������� �� ���������
  halt
end
%echo% ��� �� ���� �������� ��� ������ ���������� �����, � ������������ �� ��� � ��������� � ����ң�.
calcuid tmpobj 167031 obj
wat 167011 %purge% %tmpobj%
wat 167011 %load% obj 167032
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167011
done

~
#167036
������� �����~
2 c 1
�������~
if %arg.contains(����)%
  eval tmpvar 1
elseif %arg.contains(����)%
  eval tmpvar 2
else
  %echo% �� ��� �������� �����?
  halt
end
eval list 167033 167034 167035 167036
eval gopa %actor.all%
eval count 0
if %tmpvar% == 1
  foreach i %gopa%
    foreach m %list%
      if %i.haveobjs(%m%)% !=0
        eval count %count% + 1
        calcuid tmp %m% obj
        %send% %actor% � ��� ���� %tmp.name%
        %purge% %tmp%
      end
    done
    %echo% ����� ���� %count%
  done
  if %count% == 4
    %send% %actor% ��� ������ �� �������� ����� ����, ����� �����������, ������� �������������, ������� �������� � ��� �� ���, �������� � ������ � �������� ��������� � ��������� � Σ�
    %echoaround% %actor% ��� ������ %actor.name% �������%actor.q% ����� ����, ����� �����������, ������� �������������, ������� �������� �� ��� ����� ��������, �������� � ������ � �������� ��������� � ��������� � Σ�
    foreach i %gopa%
      %teleport% %i% 167010
    done
  end
end
if %tmpvar% == 2
  foreach i %gopa%
    foreach m %list%
      if %i.haveobjs(%m%)% !=0
        eval count %count% + 1
        calcuid tmp %m% obj
        %send% %actor% � ��� ���� %tmp.name%
        %purge% %tmp%
      end
    done
  done
  %echo% ����� ���� %count%
  if %count% == 4
    %echo% ��� ������ ���� �������� ����� ����, ������ ������ �������� ������, �������� ��������� ������, � ����� ��������� ��� ������ ��������� ��������, ������� ������
    %echo% "�� ��������� �����. �� ��������� ������� ����� � �������� ��� ���������� ���������� �������, �� �������� �������� ������ ����.
    %echo% ����, ��� ������� �������� � ��� � ���������� �� ���, ������ ���� ������������ ��������������, �� ����� ������ ���� ��������".
  done
  eval jgopa %actor.all%
  foreach j %jgopa%
    %damage% %j% 1000000
  done
end
end
~
#167037
������ ����~
0 f 100
~
%echo% "�� ��������� �����. �� ��������� ������� ����� � �������� ��� ���������� ���������� �������, �� �������� �������� ������ ����.
%echo% ����, ��� ������� �������� � ��� � ���������� �� ���, ������ ���� ������������ ��������������, �� ����� ������ ���� ��������".
eval gopa %actor.all%
foreach i %gopa%
  eval tmp %i.loadroom%
  %teleport% %i% %tmp%
done

~
#167038
������� �� �������������~
2 c 1
�����~

~
#167039
������� �� �������������~
2 z 100
~

~
#167040
������� �� �������������~
2 z 100
~

~
#167041
������� �� �������������~
0 d 0
~

~
#167042
��������~
1 c 3
��������~
if !%tmpcast%
  eval tmpcast 66
end
dg_cast %arg%
%send% %actor% �� �������� ������� ���������� %arg%
%echoaround% %actor% %actor.name% �������%actor.q% ������� ���������� %arg%
eval tmpcast %tmpcast% -1
global tmpcast
%send% %actor% ������� ������� ������: %tmpcast%
if %tmpcast% < 1
  %echo% ��������, ���������� ��������, ������
  %purge% %self%
end

~
#167043
����������� ��� ��������~
1 c 2
������������~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_feat(%tmparg%)% == 0
  %echo% �� �� ������ ������� ��� �����������
else
  %featturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% �������� �������: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% ������� ������� ��������� ����� ����� ���� � ���������� � ����� �����
  %purge% %self%
end

~
#167044
������ ��� ��������~
1 c 2
�������~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_skill(%tmparg%)% == 0
  %echo% �� �� ������ ������� ��� ������
else
  %skillturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% �������� �������: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% ������� ������� ��������� ����� ����� ���� � ���������� � ����� �����
  %purge% %self%
end

~
#167045
��������� ������ ��� ��������~
1 c 2
���������~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
%echo% �� ������ %tmparg%
%skilladd% %actor% %tmparg% 200
eval tmpturn %tmpturn% -1
global tmpturn
%echo% �������� �������: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% ������� ������� ��������� ����� ����� ���� � ���������� � ����� �����
  %purge% %self%
end
~
#167046
���������� ��� ��������~
1 c 2
�����������~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_spell(%tmparg%)% == 0
  %echo% �� �� ������ ������� ��� ����������
else
  %spellturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% �������� �������: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% ������� ������� ��������� ����� ����� ���� � ���������� � ����� �����
  %purge% %self%
end

~
#167047
�������� �����~
1 i 100
~
if !%tmpkill%
  eval tmpkill 6
end
%send% %actor% �� ������� �������� �����, ������� ��������� ���ң�, �������� ��������� ���� %victim.rname%
%send% %victim% ��� ���� �������� �����, ������� ��������� ���� ��������� ����
eval tmp %victim.hitp(1)%
%tmp%
eval tmpkill %tmpkill% -1
global tmpkill
%send% %actor% ������� ���������� �������: %tmpkill%
if %tmpkill% < 1
  %send% %actor% �������� ����� �������� ������ � ����� �����
  %purge% %self%
end
return 0

~
#167048
����� ��������~
1 c 2
������~
%send% %actor% ������� ������� ������: %tmpturn%

~
#167049
����� �������� �����~
1 c 2
�������~
%send% %actor% ������� ������� ������: %tmpkill%

~
#167050
����� ���������~
1 c 3
������~
%send% %actor% ������� ������� ������: %tmpcast%

~
#167051
������ �������~
1 c 3
������~
if !%tmpseal%
  eval tmpseal 169
end
dg_cast '����������' %actor%
eval tmpseal %tmpseal% -1
global tmpseal
%send% %actor% �������� �������: %tmpseal%
if %tmpseal% < 1
  %send% %actor% ������ ������� ����������� �� ��������� �������, ������� ����� ��������� ��������
  %purge% %self%
end

~
#167052
������ ������~
1 c 3
������~
%echo% ������� ���������� �������: %tmpseal%

~
#167053
restore �� ��~
1 i 100
~
if !%tmprestore%
  eval tmprestore 66
end
%send% %actor% �� ������� �������� �������, ������� ��������� ���ң�, ���� %victim.dname% ��������� ����, ��������, ���������� � ����� ���������� � �������� ���� �����
%send% %victim% � ��� ��������� �������� �������, ����� ��������� ����, ��������, ���������� � ����� ����������, � �������� ���� ���� �����
%victim.restore%
eval tmprestore %tmprestore% -1
global tmprestore
if %tmprestore% < 1
  %send% %actor% ���������� ��������, �������� ������� �������� � ����� �����
  %purge% %self%
end
return 0

~
#167054
������ ��~
1 c 3
�������~
%send% %actor% ������� ���������� �������: %tmprestore%

~
#167055
������ �� ��~
1 i 100
~
if !%tmpdispel%
  eval tmpdispel 66
end
%send% %actor% �� ������� �������� �����, ��� ��������� ���ң� � ������� %victim.vname% ������ ������ �����, ������ ��� �������
%send% %victim% � ��� ����������� �������� �����, ������� ������� ��� ������ ������ �����, ��������� ��� ���������� �� ��� �������
%victim.dispel%
eval tmpdispel %tmpdispel% -1
global tmpdispel
%send% %actor% �������� �������: %tmpdispel%
if %tmpdispel% < 1
  %send% %actor% �������� ����� �������� ����� � ����� �����
  %purge% %self%
end
return 0

~
#167056
������ �� ��~
1 c 3
�������~
%echo% ������� ���������� �������: %tmpdispel%

~
#167057
������ ������~
0 d 1
������� ������� �������� ������� �������� ������� ��� ����� ������� ����~
eval tmp %random.6%
�������� �� � ������ �������� �������, ���������, ��������� �� ��������
%echo% � ������� ��������� ������� ������������ �����, �������, �������� ��������� ��������, ������������ �� ���
%echo% �� ������� ţ ����� �� ���� ��� ��������� ����� %tmp%
��� %tmp%, �� ��� ��
if %tmp% == 1
  %load% obj 167027
  ���� ��� %actor.name%
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
elseif %tmp% == 2
  %load% obj 167028
  ������� ���
  %force% %actor% ����� ���
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
elseif %tmp% == 3
  %load% obj 167030
  ���� ��� %actor.name%
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
elseif %tmp% == 4
  %load% obj 167042
  ���� ��� %actor.name%
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
elseif %tmp% == 5
  %load% obj 167043
  ������� ���
  %force% %actor% ����� ���
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
elseif %tmp% == 6
  %load% obj 167044
  ������� ���
  %force% %actor% ����� ���
  ��� � ������ ��������
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% ������%actor.q%
end

~
#167058
������� � ������� ������~
0 f 100
~
%echo% ��� ������ ��� ����ӣ� ��������� ����, �� ����� ���� ��������� �������, ������� ������� ���� ����-��
%teleport% %actor% 167021
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167021
done

~
#167059
��������� ������� ����~
1 c 1
�������~
eval arg1 %arg.car%
eval victim %arg1.id%
eval arg2 %arg.cdr%
set pos %arg2%
if %arg2% == �������
  set pos 1
elseif %arg2% == ��� ��������
  set pos 2
elseif %arg2% == � ��������
  set pos 3
elseif %arg2% == ����
  set pos 4
elseif %arg2% == ��������
  set pos 5
elseif %arg2% == �����
  set pos 6
elseif %arg2% == ���������
  set pos 7
elseif %arg2% == �����
  set pos 8
else
  %send% %actor% ���������� �������, ����������� �����
  set pos 8
end
%victim.position(%pos%)%
%send% %actor% ��� %victim.rname% ������� ����������� � %arg2% (%pos%)
if %pos% == 1
  %send% %victim% �������� �� �������� � ������������ ������
elseif %pos% == 2
  %send% %victim% �������� �� ��� ������� ���
elseif %pos% == 3
  %send% %victim% �������� �� ����� � �������
elseif %pos% == 4
  %send% %victim% �� � ����  �� � ���� �� ������
elseif %pos% == 5
  %send% %victim% �������� ��� ���� �� ������� ���������
elseif %pos% == 6
  %send% %victim% ����� ���� ���� �����������, � �� ������� �� �������
elseif %pos% == 7
  %send% %victim% �������� �� ����� �������� �������� ������. �������, �� ������ ��?
elseif %pos% == 8
  %send% %victim% �� ������. ������ ������. ��� � ���.
end

~
#167060
������~
2 c 1
������~
%send% %actor% ��������� ������
%send% %actor% ����:
%send% %actor% �������������� ������: ��� �������� ������, ����������� � ������� ����
%send% %actor% �������3 ������: ��� �������� ��������� ������
%send% %actor% �������10 ������: ��� �������� �������ң���� ������
%send% %actor% �������100 ������: ��� �������� ������ ������������� � ��� ���
%send% %actor% ����� ������ (����������): ��� �������� ������, ����������� � ������� ����
%send% %actor% ������� ������: ������������ ������� ��������� �������, ���� ��������
%send% %actor% ����:
%send% %actor% �������: ������: ��� �������� ������ �����������
%send% %actor% ����������: ������������ ���� ��� ���� ������ ��������� ����� �� ������, ��� ���� ����� ������, ���� ������� ���� ��� �� ������. ���������� ����������� ���� � ������ �������
%send% %actor% �������� ����

~
#167061
��������������~
2 c 1
��������������~
if !%balans%
  eval balans 0
end
eval  rnd %random.10%
%send% %actor% ���� ������: %arg% ���
if %actor.gold% < %arg%
  %echo% � � ��� ������� � ���, ������� �����, ��������!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% �� ޣ, ������� ����������� ���� ���������? � �� ������� �� ������?
  halt
end
if %rnd% < 5
  eval casino %arg%*3/2
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% �� �������� %casino% ���
  %send% %actor% ����� ���: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% �������%actor.g% %casino% ���
else
  %send% %actor% �� ��������� %arg% ���
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% �������� ���: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% ���
  eval balans %balans% +%arg%
  remote balans %self%
end
%send% %actor% �ݣ ���������?

~
#167062
�������3~
2 c 1
�������3~
eval rnd %random.4%
%send% %actor% ���� ������: %arg% ���
if %actor.gold% < %arg%
  %echo% � � ��� ������� � ���, ������� �����, ��������!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% �� ޣ, ������� ����������� ���� ���������? � �� ������� �� ������?
  halt
end
if %rnd% == 1
  eval casino %arg%*3
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% �� �������� %casino% ���
  %send% %actor% ����� ���: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% �������%actor.g% %casino% ���
else
  %send% %actor% �� ��������� %arg% ���
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% �������� ���: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% ���
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% �ݣ ���������?

~
#167063
�������10~
2 c 1
�������10~
eval rnd %random.11%
%send% %actor% ���� ������: %arg% ���
if %actor.gold% < %arg%
  %echo% � � ��� ������� � ���, ������� �����, ��������!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% �� ޣ, ������� ����������� ���� ���������? � �� ������� �� ������?
  halt
end
if %rnd% == 1
  eval casino %arg%*10
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% �� �������� %casino% ���
  %send% %actor% ����� ���: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% �������%actor.g% %casino% ���
else
  %send% %actor% �� ��������� %arg% ���
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% �������� ���: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% ���
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% �ݣ ���������?

~
#167064
�������100~
2 c 1
�������100~
eval rnd %random.100%
%send% %actor% ���� ������: %arg% ���
if %actor.gold% < %arg%
  %echo% � � ��� ������� � ���, ������� �����, ��������!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% �� ޣ, ������� ����������� ���� ���������? � �� ������� �� ������?
  halt
end
if %rnd% == 1
  eval casino %arg%*100
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% �� �������� %casino% ���
  %send% %actor% ����� ���: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% �������%actor.g% %casino% ���
else
  %send% %actor% �� ��������� %arg% ���
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% �������� ���: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% ���
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% �ݣ ���������?

~
#167065
�����~
2 c 1
�����~
%send% %actor% �� ������ ������� � ����� �� %arg% ��� � ������� � ������ �� ������, �� ������� ������ ��� ������������ ������
if %arg% < 1
  %send% %actor% ��������� ���� ������� �������� ��� �� ������� � ��� ����� ����� ������ �� ������� �������� ������� ���
  halt
end
eval multy %random.6%
eval multy2 %random.6%
%send% %actor% �� ����� ���� �� �������, �������� ��� � ������� � ������� �� ����: ������ %multy%
%send% %actor% ������ ����� ������� � ������, ��������������� ���� ��� � ��������� �� ����: ������ %multy2%
if %actor.gold% < %arg%
  %send% %actor% ����� �� ������, ��� � ��� ��� ��� �����, �� ������� �� ������������ ������ ������
  %send% %actor% ��� ���������� ����������� ��� ����� �� �����, ��� ��� � ���������
  %teleport% %actor% %actor.loadroom%
  halt
end
if %multy% == %multy2%
  %send% %actor% �����: ������ ��
elseif %multy% > %multy2%
  eval multy3 %arg%*3/2
  eval multy4 %actor.gold(+%multy3%)%
  %send% %actor% �� �������� %multy3% ���
  %send% %actor% ����� ���: %multy4% %actor.gold%
  wat 167023 %echo% %actor.name% �������%actor.g% %multy3% ���
elseif %multy% < %multy2%
  eval multy5 %actor.gold(-%arg%)%
  %send% %actor% �� ��������� %arg% ���
  %send% %actor% �������� ���: %multy5% %actor.gold%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% ���
  eval balans %balans% +%arg%
  global balans
end

~
#167066
�������2 �� ����~
2 c 1
�������~
if !%obalans%
  eval obalans 0
end
eval rnd %random.3%
%send% %actor% ���� ������: %arg% �����
if %actor.exp% < %arg%
  %send% %actor% � � ��� ������� � ���, ������� �����, ��������!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% �� ޣ, ������� ����������� ���� ���������? � �� ������� �� ������?
  halt
end
if %rnd% == 1
  eval casino %arg%*2
  eval casino2 %actor.exp(+%casino)%
  %send% %actor% �� �������� %casino% �����
  %send% %actor% ����� �����: %casino2% %actor.exp%
  wat 167023 %echo% %actor.name% �������%actor.g% %casino% �����
else
  eval casino3 %actor.exp(-%arg%)%
  %send% %actor% �� ��������� %arg% �����
  %send% %actor% ����� �����: %casino3% %actor.exp%
  wat 167023 %echo% %actor.name% ��������%actor.g% %arg% �����
  eval obalans %obalans% +%arg%
  remote obalans %self%
end
%send% %actor% �ݣ ���������?

~
#167067
������� � ������~
2 c 100
������~
calcuid krupye 167005 mob
if %arg.contains(�����)%
  if %actor.gold% < 1500000
    %send% %actor% ��� ��� �������� ����������� �����, ��� � ���������
  else
    %send% %actor% �� �������� ������� ������ �����, ��������� ������ ������� �������������� ������
    %actor.gold(-1500000)%
    exec 167069 %krupye%
    %echoaround% %actor% %actor.name% ��������%actor.g% ����� �������
    eval balans %balans% +1500000
    global balans
  end
elseif %arg.contains(�����)%
  if %actor.gold% < 10000
    %send% %actor% �� �� � ��������� ������ ���, � ������ ����� �� ����
  else
    %send% %actor% �� ������ ������ ������������� ��������� ����� � ��������� ������ ������
    %actor.gold(-10000)%
    exec 167069 %krupye%
    %echoaround% %actor% %actor.name% ��������%actor.g% ������������� ��������� �����
    eval balans %balans% +10000
    global balans
  end
else
  %send% %actor% ����� ����� ������:
  %send% %actor% ����� �������- 1500000 ��� (������ �����)
  %send% %actor% ������������� ��������� �����- 10000 ��� (������ �����)
end

~
#167068
������ �������~
2 c 1
�������~
eval wheel %random.52%
if !%arg.contains(������)%
  %send% %actor% � ޣ ������� ��, ��������� ��� ��?
  halt
end
if %actor.haveobj(167040) == 0
  %send% %actor% ��� ����� ����� ����� �������, �������� � ��� ����
  halt
else
  %send% %actor% ����� ������� �� ������� �� ������, ���������, ����� �������������� � ����������� �� ����� %wheel%
  calcuid wheelobj 167040 obj
  %purge% %wheelobj%
end
if %wheel% < 27
  %send% %actor% �� ������ �� ��������, �� ��� �� ���!
  %echoaround% %actor% %actor.name% ��������%actor.g% ������ ������� � ������ �� �������%actor.g%. ����� �������������!
  wat 167023 %echo% %actor.name% ��������%actor.g% ������ ������� � ������ �� �������%actor.g%. ����� �������������!
elseif %wheel% > 26
  eval list 92104 166113 166114 167025 167041 167045 167046 167047 167048 167049 167018 167065 167067 167068 167069 167070 167071 167072 167073 167074 167075 167076 167077 167078 set60 set61
  eval wheelarray %array.item(%list%, %random.26%)%
  if %wheelarray% == set60
    eval list 167079 167080 167081 167082 167083 167084 167085 167086 167087 167088
    eval wheelarray %array.item(%list%, %random.10%)%
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
    global wheelarray
  elseif %wheelarray% == set61
    eval list 166115 166116 166117 166118 166119 166120 166121 166122 166123 166124 166125 166126 166127
    eval wheelarray %array.item(%list%, %random.12%)%
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
    global wheelobj
  else
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
  end
  %send% %actor% �� �������� %wheelobj.vname%
  %force% %actor% ����� %wheelobj.name%
  %echoaround% %actor% %actor.name% ��������%actor.g% ������ ������� � �������%actor.g% %wheelobj.vname%
  wat 167023 %echo% %actor.name% ��������%actor.g% ������ ������� � �������%actor.g% %wheelobj.vname%
end

~
#167069
������ ���� ��������� �����~
0 z 100
~
if %arg.contains(�����)%
  ��� ���, �������
  %load% obj 167040
  ���� ����� %actor.name%
elseif %arg.contains(�����)%
  ��� ����������
  %load% obj 167092
  ���� ����� %actor.name%
end

~
#167070
����� ���� 1670~
2 f 100
~

~
#167071
������ � ������~
0 d 1
�����~
eval utime %date.unix%
eval time1 %actor.getquest(167000)%
eval time2 (%utime% -%time1%)/60
if %time2% < 60
  eval time3 60 -%time2%
  ��� �� ��� � ���� ���� ������, ������� ����� %time3% �����
  ��� %actor.name%
  ��� %actor.name% �� � ���� ������� ���� ���� �����, ����� ���� ������� ��� �� ��� �������� �� ����
else
  ��� �����, ����������
  %send% %actor% %self.name% �������%actor.g% ���-��, ��������� ���������� �� ��� ������, �� ������� ���-�� ������� � ����� ��� � ���� �����
  ��� � ��� � ��� �����, �������
  %load% obj 167040
  ���� ����� %actor.name%
  %actor.setquest(167000 %date.unix%)%
end

~
#167072
�� ��� ������~
0 n 100
~
�� � ������� ���� �������, ��� � �����, ����� ��������� ������ �������, ������� �������� ���� �� ��� � �������� �����

~
#167073
���� � ������� ������~
1 c 4
������~
if !%rnd%
  eval rnd %random.999999999%
  global rnd
end
if !%try%
  eval try 66
end
%send% %actor% �� ������ ���������� ������ ��� �� ����� � ������� %arg%
eval try %try% -1
global try
if %try% < 1
  %send% %actor% ����� ���� ����� ��������� � ��������� �����, � ����� � ����� �����, ������� �� �� ����������
  %purge% %self%
  halt
end
if %arg% < %rnd%
  %send% %actor% ����: ����� ������� ���
elseif %arg% > %rnd%
  %send% %actor% ���������: ���������� ��� ��������� �������
end
%send% %actor% �������� �������: %try%
if %arg% == %rnd%
  %send% %actor% ��������� ����� ������ �� �����������, �� ����� � ������� ���� ����� ���������, ����� �ӣ ��ϣ ���������� �� ���
  %purge% %self%
end

~
#167074
����� ����~
0 c 100
��������~
%send% %actor% �� �������� � ������, � �������� ���� ������ ������ �����������, � ����� ��������� � ��������� � ���� ����
if %actor.sex% == 1
  %echoaround% %actor% %actor.name% ������� � ������, �������� ���� ����� � ��� ������ ���������, � ����� ��������� � ���� � ��������� � ��� ����
elseif %actor.sex% == 2
  %echoaround% %actor% %actor.name% �������� � ������, �������� ���� ����� � ��� ������ �����������, � ����� ��������� � ��� � ��������� � �� ����
end
%purge% %self%

~
#167075
������ ������~
1 c 1
�������~
%send% %actor% �� ������� �����, � ����� ���� ��������� �������� �������� ����
%load% mob 167007
%echoaround% %actor% %actor.name% ������, � ����� � ��� ��������� �������� �������� ����
calcuid tucha 167007 mob
attach 167074 %tucha%

~
#167076
������ �����������~
1 c 3
��������~
%send% %actor% ��������� ������ �������, ������ ����������� �������� ���� ���� � ����� �����
%actor.loadroom(%actor.realroom%)%
eval gopa %actor.group%
foreach i %gopa%
  %send% %i% ��������� ������� %actor.rname%, ������ ����������� �������� ���� ���� � ����� �����
done
foreach i %gopa%
  %i.loadroom(%actor.realroom%)%
done
%purge% %self%

~
#167077
�������������~
1 c 3
���������~
eval victim %arg.id%
eval victimvnum %arg.vnum%
%echo% ���: %victim.name%, ID: %victim%, VNum: %victimvnum%, UID: %victim.uniq%
if %victim% == %actor% && %actor.name% != ������
  %send% %actor% ������������� ���������, �������� ����������.
  halt
end
if %victim.realroom% != %actor.realroom% && %actor.name% != ������
  %echo% ���� �� �������, ��������� �������
  halt
end
eval victimrealroom %victim.realroom%
if %victimrealroom% < 1
  %send% %actor% ����� ���� �� ����������, ��������� �������.
  halt
end
%send% %victim% ����������� �� ������������� ��� ��������������� ���!!!
eval goldnow %victim.gold%
eval gold %victim.gold(-%goldnow%)%
%echo% ���� � �����. �����. ������� %goldnow%. �������������, �������� %gold%
eval banknow %victim.bank%
eval bank %victim.bank(-%banknow%)%
%echo% ���� � �����. �����. ������� %banknow%. �������������, �������� %bank%
eval hryvnnow %victim.hryvn%
eval hryvn %victim.hryvn(-%hryvnnow%)%
%echo% ������. �����. ������� %hryvnnow%. �������������, �������� %hryvn%
eval expnow %victim.exp% -1
eval exp %victim.exp(-%expnow%)%
%echo% ����������� ����. �����. ������� %expnow%. �������������, �������� %exp% %victim.exp%
%echo% ����. �����.
foreach i %victim.objs%
  %echo% ����������. %i.name%. �������������.
  %purge% %i%
done
%echo% ����������. �����. �������.
%force% %victim% ����� ���
foreach i %victim.objs%
  %echo% ����������. %i.name%. �������������.
  %purge% %i%
done
eval dmg %victim.hitp% +15
%echo% ��������. �����. �������. ����������� ������: %dmg%. �������������.
eval renta %victim.loadroom(4056)%
%echo% ��������� ����� �����������. �������. ������� �������: %renta% %victim.loadroom%.
%damage% %victim% %dmg%
%echo% ������������� ������ �������.

~
#167078
�������� ����������~
1 c 7
��������~
eval object %arg.id%
eval objectvnum %object.vnum%
%echo% object = %object%, objectvnum = %objectvnum%
if %object.type% == 17 || %object.type% == 19
  %send% %actor% �� ��������, ���� ������� ��� ��������-���������� %arg.vname%, � ���� ����������
  %load% obj %objectvnum%
  %echoaround% %actor% ������� �������� ���������� %arg.vname%
else
  %send% %actor% ��� ���� �� ���
end

~
#167079
������ ���������~
1 c 1
������~
eval victim %arg.id%
if %victim.realroom% < 1
  %send% %actor% ���� �� �������
  halt
end
%send% %actor% ���� �������, ������ ��������:
%send% %actor% ID: %victim%, VNum: %victim.vnum%, UID: %victim.uniq%
%send% %actor% ���: %victim.name% %victim.rname% %victim.dname% %victim.vname% %victim.tname% %victim.pname%
%send% %actor% ���:
if %victim.sex% == 1
  %send% %actor% �������
elseif %victim.sex% == 2
  %send% %actor% �������
elseif %victim.sex% == 0
  %send% %actor% �������
elseif %victim.sex% == 3
  %send% %actor% ������������� �����
else
  %send% %actor% �� ����������
end
%send% %actor% �������: %victim.age% ���
%send% %actor% ���������������:
if %victim.religion% == 0
  %send% %actor% ���������
elseif %victim.religion% == 1
  %send% %actor% ������������
else
  %send% %actor% �� ����������
end
%send% %actor% ���:
if %victim.race% == 0
  %send% %actor% ��������
elseif %victim.race% == 1
  %send% %actor% ������
elseif %victim.race% == 2
  %send% %actor% �������
elseif %victim.race% == 3
  %send% %actor% ������
elseif %victim.race% == 4
  %send% %actor% ������
elseif %victim.race% == 5
  %send% %actor% ��������
else
  %send% %actor% �� ����������
end
if %victim.clan% == 0
  %send% %actor% � ����� �� �������
else
  %send% %actor% ������� � �����: %victim.clan%
end
%send% %actor% ���������� � �����/����: %victim.align%
%send% %actor% ������� ��������: %victim.level%
%send% %actor% ����������� ����: %victim.exp%
%send% %actor% ���������� ��������������: %victim.remort%
%send% %actor% ����� ���������:
if %victim.class% == 0
  %send% %actor% ������
elseif %victim.class% == 1
  %send% %actor% ������
elseif %victim.class% == 2
  %send% %actor% ����
elseif %victim.class% == 3
  %send% %actor% ��������
elseif %victim.class% == 4
  %send% %actor% �������
elseif %victim.class% == 5
  %send% %actor% ���������
elseif %victim.class% == 6
  %send% %actor% ��������
elseif %victim.class% == 7
  %send% %actor% ���������
elseif %victim.class% == 8
  %send% %actor% ������������
elseif %victim.class% == 9
  %send% %actor% ������
elseif %victim.class% == 10
  %send% %actor% �������
elseif %victim.class% == 11
  %send% %actor% ������
elseif %victim.class% == 12
  %send% %actor% �����
elseif %victim.class% == 13
  %send% %actor% �����
else
  %send% %actor% �� ����������
end
%send% %actor% �����: %victim.hitp%/%victim.maxhitp%
%send% %actor% ����: %victim.mana%/%victim.maxmana%
%send% %actor% �������: %victim.move%/%victim.maxmove%
%send% %actor% �������� ���������, ���������� +����������:
%send% %actor% ����: %victim.str% + %victim.stradd%
%send% %actor% ��: %victim.int% + %victim.intadd%
%send% %actor% ��������: %victim.wis% + %victim.wisadd%
%send% %actor% �����������: %victim.dex% + %victim.dexadd%
%send% %actor% ������������: %victim.con% + %victim.conadd%
%send% %actor% �������: %victim.cha% + %victim.chaadd%
%send% %actor% ������� ������, ��: %victim.acbase% + %victim.acadd%
%send% %actor% ��������: %victim.hr%
%send% %actor% ��������: %victim.dr%
%send% %actor% ����������: %victim.castsucc%
%send% %actor% �����: %victim.morale%
%send% %actor% ����������: %victim.initiative%
%send% %actor% ��: %victim.poison%
%send% %actor% ������: %victim.size% + %victim.sizeadd%
%send% %actor% ���: %victim.weight%
%send% %actor% ����������� ���: %victim.carry_weight%/%victim.can_carry_weight%
%send% %actor% ����������:
set num 0
while %num% <= 18
  eval eq %victim.eq(%num%)%
  if %eq% == 0
    *nop
  else
    %send% %actor% %eq.name%
  end
  eval num %num% + 1
done
%send% %actor% �������� � ���������:
foreach i %victim.objs%
  %send% %actor% %i.iname%
done
eval victimrroom %victim.realroom%
eval victimlroom %victim.loadroom%
%send% %actor% ������ ���������: %victimrroom.name% ( %victimrroom% )
if %victim.fighting% == 0
  %send% %actor% ������ �� ���������
else
  eval fighting %actor.fighting%
  %send% %actor% ������ ��������� � %fighting.tname%
end
%send% %actor% �����: %victimlroom.name% ( %victimlroom% )
if %victim.rentable% > 0
  %send% %actor% ����� ���� �� ������
else
  %send% %actor% �� ����� ���� �� ������
end
if %victim.is_killer% > 0
  %send% %actor% �������
else
  %send% %actor% �� �������
end
if %victim.is_thief% > 0
  %send% %actor% ���
else
  %send% %actor% �� ���
end
if %victim.agressor% < 1
  %send% %actor% �� ��������� � ������ ���������
else
  eval victimaroom %victim.agressor%
  %send% %actor% ��������� � ������ ���������, ����� ���������: %victimaroom.name% ( %victimaroom% )
end

~
#167080
��������� ������~
1 c 1
������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
if %arg1% == 0
  %send% %actor% ������ <���� �������> <����� �������� � �� �������>
  %send% %actor% ���� ������ ������, �� ������� �������� ���� ��������� 0
  halt
end
if %world.room(%arg1%)% == 0
  %send% %actor% ����� ������� �� ����������
  halt
end
%portal% %arg1% %arg2%
oat %arg1% %portal% %actor.realroom% %arg2%

~
#167081
�������� ���������� �������~
2 z 100
~
%portal% %arg%
wat %arg1% %portal% %actor.realroom% %arg2%
calcuid tmproom2 %arg1% room
detach 167081 %tmproom2%

~
#167082
���� �������~
1 c 3
����~
eval timestoper %actor%
%send% %actor% �� �������� ��������, � ��� ������ �������
%echoaround% %actor% %actor.name% �������%actor.g% ��������, �ӣ ������ ������� � �� � ��� �����
calcuid tmproom %actor.realroom% room
attach 167083 %tmproom%
attach 167084 %tmproom%
foreach i %actor.all%
  if %i% == %actor%
    halt
  else
    dg_affect %i% ���������� ���������� 1 26 7
  done
end
run 167084 %tmproom%

~
#167083
������ ��������� �������~
2 eg 100
~
%send% %actor% �� ���������� ������ ������, �� ��������� ���� �� ���� ��� ����� �������
%echoaround% %actor% %actor.name% ����������%actor.g% ������ ������, �� ������ �� ����� ������� �� �����
return 0

~
#167084
���� � ��������� �������~
2 abz 100
~
foreach i %actor.all%
  if %i% == %timestoper%
  else
    dg affect %i% ���������� ���������� 1 26 7
  end
done

~
#167085
����� �������~
1 c 3
�����~
%send% %actor% �� �������� ��������, � �ӣ ������ ����� �����������
%echoaround% %actor% %actor.name% �������%actor.g% ��������, � �� ������� �� ������������� ������ ��� ����� �����
calcuid tmproom %actor.realroom% room
detach 167083 %tmproom%
detach 167084 %tmproom%

~
#167086
������ ��� �����~
1 c 3
/~
%arg%

~
#167087
������ �� ��������~
1 c 3
�������~
eval arg1 %arg%
calcuid tmproom %actor.realroom% room
attach 167088 %tmproom%
exec 167088 %tmproom%
detach 167088 %tmproom%

~
#167088
������ ����� exec~
2 z 100
~
wworldecho �������� ����� ���� ��������� ���� �����:
wworldecho %arg1%

~
#167089
������������������� ������ ���������~
1 c 3
�������~
calcuid pocket 167060 obj
calcuid tmproom %actor.realroom% room
%send% %actor% ������������������� ������ �� ����� ����� ��������, ������ ������ ���� ��� ���������� �������� � ��������.
%echoaround% %actor%  ������������������� ������ �� ����� %actor.rname% ��������, ������ � ���� ��� ���������� �������� � ����� ��������
foreach i %tmproom.all%
  foreach k %tmproom.objects%
    %k.put(%pocket%)%
  done
  %force% %i% ����� ���
  foreach j %i.objs%
    %j.put(%pocket%)%
  done
done

~
#167090
���������������� ���~
1 c 3
�������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval sendobjvnum %arg2.vnum%
calcuid sendobj %sendobjvnum% obj
%send% %actor% �� ������ ������� %victim.dname% %sendobj.vname% ���������������� �����
if %victim% == %actor%
  %send% %actor% ���������� ������� ������ ����? ������������!
  halt
end
if %victim.vnum% != -1
  %send% %actor% ���������� ������� ����� ������ �������
  halt
end
if %victim.haveobj(%self.vnum%)% == 0
  %send% %actor% %victim.dname% ����� ������� ��� ���������������� ���
  halt
end
if %actor.haveobj(%sendobjvnum%)% == 0
  %send% %actor% �� ���� � ��� ���� ���� ����!
  halt
else
  %purge% %sendobj%
  %load% obj %sendobjvnum%
  calcuid sendobj %sendobjvnum% obj
  %sendobj.put(%victim%)%
  %send% %actor% ������� ������� ����������
  %send% %victim% �� ����������������� ���� � ��� ������ ������� �� %actor.rname%
end

~
#167091
��������� dg_affect~
1 c 3
�����~
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
eval arg3 %arg.words(3)%
eval arg4 %arg.words(4)%
eval arg5 %arg.words(5)%
eval arg6 %arg.words(6)%
eval victim %arg1.id%
dg_affect %victim% %arg2% %arg3% %arg4% %arg5% %arg6%
%send% %actor% �� ������� ����� � ����������� %victim.rname%
%send% %victim% %actor.name% ������ ����� � ���� �������, � �� ������������� ���� �����
foreach i %actor.all%
  if %i% == %actor% || %i% == %victim%
    halt
  else
    %send% %i% %actor.name% ������ ����� � ������� %victim.rname%
  done
end

~
#167092
dg_affect �� ����~
1 c 3
��������~
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
eval arg3 %arg.words(3)%
eval arg4 %arg.words(4)%
eval arg5 %arg.words(5)%
%send% %actor% �� �������� ���� ������ � ������� �� � �������, �������� �� ����, ����������� �����
%echoaround% %actor% %actor.name% ������� ���� ����� ����� � ������ �� � �������, �������� �� ���� ������, � ��� � ��� �����, ����� ���� �� ������������� ���� �����
foreach i %actor.all%
  if %i% == %actor%
    halt
  else
    dg_affect %i% %arg1% %arg2% %arg3% %arg4% %arg5%
  done
end

~
#167093
��������� ��������� �����~
1 c 1
�����~
set arg1 %arg.car%
set arg2 %arg.words(2)%
set arg3 %arg.words(3)%
eval victim %arg1.id%
%victim.hitp(%arg2%%arg3%)%
%send% %actor% � %victim.rname% ����� ����������� � %victim.hitp%

~
#167094
������ ����~
1 c 3
����~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval mp %victim.mana(%arg2%)%
%send% %actor% � %victim.rname% ���� ����������� � %mp%

~
#167095
������ �������~
1 c 3
�������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval ep %victim.move(%arg2%)%
%send% %actor% � %victim.rname% ������� ����������� � %ep%

~
#167096
������ ����~
1 c 3
����������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval cast %victim.castsucc(%arg2%)%
%send% %actor% � %victim.rname% ���������� ����������� � %cast%

~
#167097
������ ������~
1 c 3
������~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval size %victim.sizeadd(%arg2%)%
%send% %actor% � %victim.rname% ������ ���������� � %victim.size% ���������� + %size% ����������

~
#167098
������ ����~
1 c 3
����~
set arg1 %arg.words(1)%
set arg2 %arg.words(2)%
calcuid tmproom %arg1% room
set flag %tmproom.flag(%arg2%)%
%echo% ��� ������� %arg1% ������ ���� %arg2% � %flag%

~
#167099
�����~
1 c 3
�����~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval lroom %victim.loadroom(%arg2%)%
%send% %actor% � %victim.rname% ������� ����� ����������� � %lroom%

~
$
$
