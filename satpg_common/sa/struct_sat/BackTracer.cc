
/// @file BackTracer.cc
/// @brief BackTracer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BackTracer.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
// クラス BackTracer
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] val_map ノードの値を保持するクラス
BackTracer::BackTracer(ymuint max_id,
		       const ValMap& val_map) :
  mAlloc(sizeof(NodeList), 1024),
  mMaxId(max_id),
  mJustArray(max_id, nullptr),
  mJust0Array(max_id, nullptr),
  mTfoMark(max_id, false),
  mMarkArray(max_id, 0U),
  mValMap(val_map)
{
}

// @brief デストラクタ
BackTracer::~BackTracer()
{
}

// @brief バックトレースを行なう．
// @param[in] suf_list 十分割り当てのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
void
BackTracer::run(const NodeValList& suf_list,
		NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // suf_list の値を正当化する．
  for (ymuint i = 0; i < suf_list.size(); ++ i) {
    NodeVal nv = suf_list[i];
    const TpgNode* node = nv.node();
    NodeList* node_list = nullptr;
    node_list = justify(node);
  }

  // 故障差の伝搬している外部出力を選ぶ．
  ymuint nmin = 0;
  NodeList* best_list = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node) != fval(node) ) {
      // 正当化を行う．
      NodeList* node_list = justify(node);
      ymuint n = list_size(node_list);
      if ( nmin == 0 || nmin > n ) {
	nmin = n;
	best_list = node_list;
      }
    }
  }
  ASSERT_COND( nmin > 0 );

  for (NodeList* tmp = best_list; tmp; tmp = tmp->mLink) {
    const TpgNode* node = tmp->mNode;
    int time = tmp->mTime;
    record_value(node, time, pi_assign_list);
  }
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
BackTracer::NodeList*
BackTracer::justify(const TpgNode* node)
{
  if ( justified_mark(node) ) {
    return mJustArray[node->id()];
  }
  set_justified(node);

  if ( node->is_ppi() ) {
    // val を記録
    mJustArray[node->id()] = new_list_cell(node, 0);
    return mJustArray[node->id()];
  }

  Val3 gval = this->gval(node);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just_sub1(node);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just_sub1(node);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
BackTracer::NodeList*
BackTracer::just_sub1(const TpgNode* node)
{
  NodeList*& node_list = mJustArray[node->id()];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify(inode);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val 値
BackTracer::NodeList*
BackTracer::just_sub2(const TpgNode* node,
		      Val3 val)
{
  ymuint ni = node->fanin_num();
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 ival = gval(inode);
    if ( ival != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode);
    ymuint n = list_size(node_list1);
    if ( min == 0 || min > n ) {
      pos = i;
      min = n;
    }
  }

  ASSERT_COND ( pos < ni );

  NodeList*& node_list = mJustArray[node->id()];
  list_merge(node_list, mJustArray[node->fanin(pos)->id()]);
  return node_list;
}

// @brief 新しいリストのセルを返す．
BackTracer::NodeList*
BackTracer::new_list_cell(const TpgNode* node,
		       int time)
{
  void* p = mAlloc.get_memory(sizeof(NodeList));
  NodeList* tmp = new (p) NodeList;
  tmp->mNode = node;
  tmp->mTime = time;
  tmp->mLink = nullptr;
  return tmp;
}

// @brief リストをマージする．
void
BackTracer::list_merge(NodeList*& dst_list,
		    NodeList* src_list)
{
  NodeList** pdst = &dst_list;
  NodeList* src = src_list;
  while ( *pdst != nullptr && src != nullptr ) {
    if ( (*pdst)->mNode->id() < src->mNode->id() ) {
      pdst = &(*pdst)->mLink;
    }
    else if ( (*pdst)->mNode->id() > src->mNode->id() ) {
      NodeList* tmp = new_list_cell(src->mNode, src->mTime);
      NodeList* next = *pdst;
      *pdst = tmp;
      tmp->mLink = next;
      pdst = &tmp->mLink;
      src = src->mLink;
    }
    else {
      pdst = &(*pdst)->mLink;
      src = src->mLink;
    }
  }
  for ( ; src != nullptr; src = src->mLink) {
    NodeList* tmp = new_list_cell(src->mNode, src->mTime);
    *pdst = tmp;
    pdst = &tmp->mLink;
  }
}

// @brief リストのサイズを返す．
ymuint
BackTracer::list_size(NodeList* node_list)
{
  ymuint n = 0;
  for (NodeList* tmp = node_list; tmp; tmp = tmp->mLink) {
    ++ n;
  }
  return n;
}

// @brief リストを削除する．
void
BackTracer::list_free(NodeList* node_list)
{
  for (NodeList* tmp = node_list; tmp; ) {
    NodeList* next = tmp->mLink;
    mAlloc.put_memory(sizeof(NodeList), tmp);
    tmp = next;
  }
}

// @brief ノードの TFO に印をつける．
void
BackTracer::mark_tfo(const TpgNode* node)
{
  if ( mTfoMark[node->id()] ) {
    return;
  }
  mTfoMark[node->id()] = true;

  ymuint nfo = node->fanout_num();
  for (ymuint i = 0; i < nfo; ++ i) {
    const TpgNode* onode = node->fanout(i);
    mark_tfo(onode);
  }
}

END_NAMESPACE_YM_SATPG_SA
