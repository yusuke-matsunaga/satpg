
/// @file MultiExtractor.cc
/// @brief MultiExtractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MultiExtractor.h"
#include "TpgFault.h"
#include "TpgNode.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

Expr
extract_all(const TpgNode* root,
	    const VidMap& gvar_map,
	    const VidMap& fvar_map,
	    const vector<SatBool3>& model)
{
  MultiExtractor extractor(gvar_map, fvar_map, model);
  return extractor.get_assignments(root);
}

BEGIN_NONAMESPACE

int debug = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MultiExtractor
//////////////////////////////////////////////////////////////////////

// @param[in] gvar_map 正常値の変数番号のマップ
// @param[in] fvar_map 故障値の変数番号のマップ
// @param[in] model SATソルバの作ったモデル
MultiExtractor::MultiExtractor(const VidMap& gvar_map,
			       const VidMap& fvar_map,
			       const vector<SatBool3>& model) :
  mGvarMap(gvar_map),
  mFvarMap(fvar_map),
  mSatModel(model)
{
}

// @brief デストラクタ
MultiExtractor::~MultiExtractor()
{
}

// @brief 各出力へ故障伝搬する値割り当てを求める．
// @param[in] root 起点となるノード
// @return 複数の値割り当てを表す論理式を返す．
Expr
MultiExtractor::get_assignments(const TpgNode* root)
{
  // root の TFO (fault cone) に印をつける．
  // 同時に故障差の伝搬している外部出力のリストを作る．
  mFconeMark.clear();
  mark_tfo(root);
  ASSERT_COND( mSpoList.size() > 0 );

  mExprMap.clear();

  // 故障差の伝搬している経路を探す．
  Expr expr = Expr::const_zero();
  for ( auto spo: mSpoList ) {
    // spo に到達する故障伝搬経路の
    // side input の値を記録する．
    expr |= record_sensitized_node(spo);
  }

#if 0
  if ( debug ) {
    ostream& dbg_out = cout;
    dbg_out << "Extract at Node#" << root->id() << endl;
    for ( auto& assign_list: assignment_list ) {
      const char* comma = "";
      for ( auto nv: assign_list ) {
	const TpgNode* node = nv.node();
	dbg_out << comma << "Node#" << node->id()
		<< ":";
	if ( nv.val() ) {
	  dbg_out << "1";
	}
	else {
	  dbg_out << "0";
	}
	comma = ", ";
      }
      dbg_out << endl;
    }
  }
#endif

  return expr;
}

// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
void
MultiExtractor::mark_tfo(const TpgNode* node)
{
  if ( mFconeMark.check(node->id()) ) {
    return;
  }
  mFconeMark.add(node->id());

  if ( node->is_ppo() ) {
    if ( gval(node) != fval(node) ) {
      mSpoList.push_back(node);
    }
  }

  for ( auto onode: node->fanout_list() ) {
    mark_tfo(onode);
  }
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
// @param[in] node 対象のノード
Expr
MultiExtractor::record_sensitized_node(const TpgNode* node)
{
  ASSERT_COND( gval(node) != fval(node) );

  Expr expr;
  if ( !mExprMap.find(node->id(), expr) ) {
    // * 故障差の伝搬しているファンインには record_sensitized_node() を呼ぶ．
    // * そうでない fault-cone 内のファンインには record_masking_node() を呼ぶ．
    // * それ以外のファンインには record_side_input() を呼ぶ．
    expr = Expr::const_one();
    for ( auto inode: node->fanin_list() ) {
      Expr expr1;
      if ( mFconeMark.check(inode->id()) ) {
	// fault-cone 内部のノード
	if ( gval(inode) != fval(inode) ) {
	  expr1 = record_sensitized_node(inode);
	}
	else {
	  expr1 = record_masking_node(inode);
	}
      }
      else {
	// fault-cone 外部のノード
	expr1 = record_side_input(inode);
      }
      expr &= expr1;
    }
    mExprMap.add(node->id(), expr);
  }

  return expr;
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
// @param[in] node 対象のノード
Expr
MultiExtractor::record_masking_node(const TpgNode* node)
{
  ASSERT_COND ( gval(node) == fval(node) );

  Expr expr;
  if ( !mExprMap.find(node->id(), expr) ) {
    // ファンインには sensitized node があって
    // side input がある場合．
    bool has_cval = false;
    bool has_snode = false;
    vector<const TpgNode*> cnode_list;
    for ( auto inode: node->fanin_list() ) {
      if ( mFconeMark.check(inode->id()) ) {
	if ( gval(inode) != fval(inode) ) {
	  // このノードには故障差が伝搬している．
	  has_snode = true;
	}
      }
      else {
	if ( node->cval() == gval(inode) ) {
	  // このノードは制御値を持っている．
	  has_cval = true;
	  cnode_list.push_back(inode);
	}
      }
    }
    if ( has_snode && has_cval ) {
      // node のファンインに故障差が伝搬しており，
      // 他のファンインの制御値でブロックされている場合，
      // その制御値を持つノードの値を確定させる．
      expr = Expr::const_zero();
      for ( auto cnode: cnode_list ) {
	expr |= record_side_input(cnode);
      }
    }
    else {
      // ここに来たということは全てのファンインに故障差が伝搬していないか
      // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
      // に再帰する．
      expr = Expr::const_one();
      for ( auto inode: node->fanin_list() ) {
	if ( mFconeMark.check(inode->id()) ) {
	  if ( gval(inode) != fval(inode) ) {
	    expr &= record_sensitized_node(inode);
	  }
	  else {
	    expr &= record_masking_node(inode);
	  }
	}
	else {
	  expr &= record_side_input(inode);
	}
      }
    }
    mExprMap.add(node->id(), expr);
  }

  return expr;
}

END_NAMESPACE_YM_SATPG
