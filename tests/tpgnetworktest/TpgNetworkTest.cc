
/// @file TpgNetworkTest.cc
/// @brief TpgNetworkTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_YM_SATPG

void
loop_test(ymuint loop_count,
	  const string& filename)
{
  for (ymuint i1 = 0; i1 < loop_count; ++ i1) {
    TpgNetwork network;
    for (ymuint i2 = 0; i2 < loop_count; ++ i2) {
      cout << "            "
	   << "\r" << i1 << ": " << i2 << "\r";
      cout.flush();
      if ( !network.read_iscas89(filename) ) {
	cerr << "Error in reading " << filename << endl;
	break;
      }
      TpgFaultMgr fmgr(network);
    }
  }
  cout << endl;
}

END_NAMESPACE_YM_SATPG

int
main(int argc,
     char** argv)
{
  using namespace std;

  if ( argc != 2 ) {
    cerr << "USAGE: " << argv[0] << " filename" << endl;
    return 255;
  }

  nsYm::nsSatpg::loop_test(100, argv[1]);
}
