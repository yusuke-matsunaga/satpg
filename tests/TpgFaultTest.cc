
/// @file TpgFaultTest.cc
/// @brief TpgFaultTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgStemFault.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

int
TpgFaultTest(int argc,
	     char** argv)
{
  TpgNode* node = TpgNode::make_logic(0, kGateCONST0, vector<TpgNode*>(), 1);
  for (ymuint i = 0; i < 1000000000; ++ i) {
    TpgStemFault* fault = new TpgStemFault(0, nullptr, 0, node, nullptr);
    delete fault;
  }

  return 0;
}

END_NAMESPACE_YM_SATPG


int
main(int argc,
     char** argv)
{
  nsYm::nsSatpg::TpgFaultTest(argc, argv);
}
