#9900
����� � ����-��������~
0 r 100
~
If %exist.obj(9986)% == 1
  wait 1s
  say ��-��-��, ����� ������ �����. ���� � ���� � ������ ���� �����, � ��� �������.
  wait 1s
  say � ����� ���-�� ����� �� ��� � ������� ���� ����: �� �� �� �����, �� �� � �������.
  wait 1s
  say ������ ���! ����� ��� ����, � � � ����� �� ��������.
else
  Halt
end
~
#9901
����� ������� ����-��������~
0 j 9986
100~
wait 1s
if %object.vnum% == 9986
  say �������������, �� ���� �������%actor.g%. ������ � ���� ���! 
  switch %random.3%
    case 1
      say ��� ���� ������� �� ���.
      mload obj 9946
      give ������ .%actor.name%
    break
    case 2
      say � ������� � ��� ���� 150 �����, ���� ����� � ����� ��������
      %self.gold(+150)%
      give 150 ���� %actor.name%
    break
    case 3
      say � ���������, ��� ����� ���� ���������.
    break
  done
  mpurge ����
else
  say ���?
  wait 2s
  say ��� ���?! ������ �� ���� ��� �����!
  wait 1s
  eval getobject %object.name%
  if %getobject.car% == ����
    mpurge ����
  else
    ���� %getobject.car%.%getobject.cdr%
  end
end
~
#9902
������ ���������~
0 d 100
������~
say ���� ��, ������, ����������� �� ������� ������� ���� �����������
say (� �����) � ��������� ��� ������ ������� �����, � ���� �������.
say ��-��-��! ��� ��!
done
~
#9903
������~
0 gh 100
~
��� ����� ����������, ����� ������!
%echo% %self.name% �����%actor.g% �����
~
#9904
������~
2 c 100
������~
if !%arg%
  %send% %actor% ������ ���� ������
  %send% %actor% ���� ���������� �� ����, ������� ������������� ���: �������100/����
  %send% %actor% ��������: ������ �� ������������ ������� �����
  halt
end
set chance %arg.car%
set bet %arg.words(2)%
if %chance% > 95 || %chance% < 1
  %send% %actor% ���� �������� �� ����� ���� ���� 95 � ���� 1
  halt
end
if %bet% < 1
  %send% %actor% ������ �� ����� ���� ���� 1
  halt
end
wat 9999 %echo% %actor.gold(-%bet%)%
eval rnd %random.100%
if %rnd% <= %chance%
  eval vin (%bet%*100)/%chance%
  eval vin2 %vin%-%bet%
  %send% %actor% �� ��������!!!
  %send% %actor% �������: %vin%, ������ �������: %vin2%, ����� ���: %actor.gold(+%vin%)%
else
  %send% %actor% �� ��������� %bet% ���
  %send% %actor% ������, � ��������� ��� �������.
end
~
#9905
���������� �������, ����������!~
2 f 100
~
if %random.100% <= 30
  eval event %random.100%
  if %event% <= 70
    eval firstvnum 9700
    eval lastvnum 9997
    eval counter %firstvnum%
    while %counter% <= %lastvnum%
      eval room %world.room(%counter%)%
      if %room%
        foreach j %room.objects%
          if %j.vnum% == 9995
            eval flag 1
          end
        done
        if !%flag% && %random.100% <= 15
          wat %counter% %load% obj 9995
        end
      end
      unset flag
      wait 2
      eval counter %counter%+1
    done
    if %random.100% <= 80
      wzoneecho 9900 &R��������, �����������! �������� ���������� ���������� ���������� ���������� ������ ��������, ������������� � ����������� ������.&n
      wzoneecho 9900 &R����� ������ ��������� � ���������� �� �������� ����� ����� �� ������������ ����������.&n
    end
  end
end
~
#9906
��������� �� ��������~
1 t 100
~
%send% %actor% �� ����������� ������ ������������� ����� ��������!!!
%echoaround% %actor% %actor.iname% ����������� �����%actor.g% ������������� ����� ��������!!!
%echo% &R��������� �� �������� ����� ������ �����!&n
foreach i %self.all%
  %send% %i% ��������� ������� ��� ����� �� �����!
  %echoaround% %i% ��������� ������� %i.vname% ����� �� �����!
  eval dmg %i.hitp%+20
  %damage% %i% %dmg%
done
%purge% %self%
~
#9950
�������� � �������~
2 c 100
������~
wait 1
%portal% 7028 1
~
#9951
�������� �� ������~
2 c 100
������~
wait 1
%portal% 21038 1
~
#9989
�����������~
0 d 100
��������~
� ������, �� ������ � �� ���� ���� ��������������� ���� ����
~
#9990
����������� �������~
0 h 100
~
wait 1s
� ������, %actor.name%
wait 1s
� ���� �� ������ ����-������ �������������, �� ����� ����� ��������.
wait 1s
� ���� ���� ����� �������� ����������, �� ��� � �����
wait 1s
��� .%actor.name%
~
#9991
������ ������~
0 d 100
��������~
if (!%arg%.contains(����))
  � ��, � �� ���������, ������� ���
  halt
end
� ���, %actor.name% ��� ������� � ������ ���� !
wait 1s
emo �������� ������� �����
dg_cast '����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '����' %actor.name%
dg_cast '����' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '���� ��' %actor.name%
dg_cast '���� ��' %actor.name%
dg_cast '��� ���' %actor.name%
wait 1s
���
� �� ��� � �ӣ
��� %actor.name%
~
#9992
���� � �����~
1 bz 100
~
done
wait 1
switch %random.12%
  Case 1
    oecho �� ������� ����� ����������� �������: "���������"
  break
  case 2
    oecho ����� ���� ������� ����������� ������: "� ������ �� �������� ���������!"
  break
  case 3
    oecho "� �����", - �������������� ������� ��� ���� ���� �������,
    oecho ����� ����� �������� ������� ����������
  break
  case 4
    oecho "�� �� ��������, �� �� ��������...", - ������ ������, �������� ���������
  case 5
    oecho ���� �������, � ����������� �����, ������ �������, ������� ����,
    oecho ���� � �������, � ������������.
  break
done
~
#9993
new trigger~
0 c 100
����~
mecho ����� � ����� ����� ��� �� ������!
dg_cast '��� �����'
done
~
#9994
prool trig 1~
2 d 100
���~
say My trigger commandlist is not complete!
say ��, ���!
~
$~
