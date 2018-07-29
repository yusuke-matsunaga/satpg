#! /usr/bin/env python3
#
# @file dtpg_test.py
# @brief Dtpg のテストプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

import argparse
#import os.path
#import time
from satpg_core import FaultType
from exec_one import exec_one

#algorithm_list = ('coloring2', 'mincov+dsatur', 'mincov+isx', 'isx+mincov', 'dsatur+mincov')
algorithm_list = ( 'coloring2', 'mincov+dsatur' )

def main() :

    parser = argparse.ArgumentParser()

    type_group = parser.add_mutually_exclusive_group()
    type_group.add_argument('--stuck_at',
                            action = 'store_true',
                            help = 'TPG for stuck-at fault [default]')
    type_group.add_argument('--transition_delay',
                            action = 'store_true',
                            help = 'TPG for transition-delay fault')
    parser.add_argument('-k',
                       default = 2,
                       help = 'k-detection [default = 2]')

    parser.add_argument('-v', '--verbose',
                        action = 'store_true',
                        help = 'make verbose')

    parser.add_argument('file_list', metavar = '<filename>', type = str,
                        nargs = '+',
                        help = 'file name')

    args = parser.parse_args()
    if not args :
        exit(1)

    if args.stuck_at :
        fault_type = FaultType.StuckAt
    elif args.transition_delay :
        fault_type = FaultType.TransitionDelay
    else :
        # デフォルト
        fault_type = FaultType.StuckAt

    verbose = args.verbose

    k = int(args.k)
    for file_name in args.file_list :
        r_dic = {}
        exec_one(file_name, fault_type, k, algorithm_list, r_dic, verbose)
        for sig, ( nv, cpu_time) in r_dic.items() :
            print('{:40}: {:5}: {:8.2f}'.format(sig, nv, cpu_time))


if __name__ == '__main__' :
    main()
