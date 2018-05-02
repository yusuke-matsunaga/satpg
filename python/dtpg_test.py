#! /usr/bin/env python3
#
# @file dtpg_test.py
# @brief Dtpg のテストプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

from dtpg import Dtpg

fault_type = FaultType.StuckAt
network = TpgNetwork.read_blif('/home/yusuke/data/public/MCNC.blifdata/C432.blif')

fsim2 = Fsim('Fsim2', network, fault_type)

dtpg = Dtpg(network, fault_type)

ndet, nunt, nabt = dtpg.ffr_mode()

print('# of total faults:      {}'.format(ndet + nunt + nabt))
print('# of detected faults:   {}'.format(ndet))
print('# of untestable faults: {}'.format(nunt))
print('# of aborted faults:    {}'.format(nabt))
