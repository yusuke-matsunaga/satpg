#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgNode TpgNode.h "TpgNode.h"
/// @brief SATPG 用のノードを表すクラス
/// @sa TpgNetwork
/// @sa TpgFault
/// @sa TpgMFFC
/// @sa TpgFFR
///
/// 基本的には一つの BnNode に対応しているが，
/// もとのゲートが組み込み型でない場合には複数の TpgNode を組み合わ
/// せてもとのゲートを表す．<br>
/// そのため，場合によってはファンインの故障を表すための仮想的な
/// ノードを挿入する場合もある．<br>
/// このクラス(の派生クラス)は TpgNodeFactory のみが生成できる．
//////////////////////////////////////////////////////////////////////
class TpgNode
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgNode(int id);

  /// @brief デストラクタ
  virtual
  ~TpgNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  int
  id() const;

  /// @brief ノード名を返す．
  const char*
  name() const;

  /// @brief 外部入力タイプの時 true を返す．
  virtual
  bool
  is_primary_input() const;

  /// @brief 外部出力タイプの時 true を返す．
  virtual
  bool
  is_primary_output() const;

  /// @brief DFF の入力に接続している出力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_output() でもある．
  virtual
  bool
  is_dff_input() const;

  /// @brief DFF の出力に接続している入力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_input() でもある．
  virtual
  bool
  is_dff_output() const;

  /// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clock() const;

  /// @brief DFF のクリア端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clear() const;

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_preset() const;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  virtual
  bool
  is_ppi() const;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  virtual
  bool
  is_ppo() const;

  /// @brief logic タイプの時 true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::ppi(node->input_id())
  /// の関係を満たす．
  /// is_ppi() が false の場合の返り値は不定
  virtual
  int
  input_id() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::ppo(node->output_id())
  /// の関係を満たす．
  /// is_ppo() が false の場合の返り値は不定
  virtual
  int
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  virtual
  int
  output_id2() const;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  TpgDff*
  dff() const;

  /// @brief ゲートタイプを得る．
  ///
  /// - is_logic() が true の時はゲートタイプを返す．
  /// - is_ppi() が true の時は GateType::INPUT を返す．
  /// - is_ppo() が true の時は GateType::BUFF を返す．
  /// - それ以外の返り値は不定
  virtual
  GateType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  noval() const;

  /// @brief ファンイン数を得る．
  virtual
  int
  fanin_num() const = 0;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(int pos) const = 0;

  /// @brief ファンアウト数を得る．
  int
  fanout_num() const;

  /// @brief ファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  TpgNode*
  fanout(int pos) const;

  /// @brief FFR の根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  ffr_root() const;

  /// @brief FFRの根の場合にFFRを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgFFR*
  ffr() const;

  /// @brief MFFCの根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  mffc_root() const;

  /// @brief MFFCの根の場合にMFFCを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgMFFC*
  mffc() const;

  /// @brief 直近の dominator を得る．
  ///
  /// これが nullptr の場合は MFFC の根のノードだということ．
  const TpgNode*
  imm_dom() const;

  /// @brief このノードに含まれる代表故障の数を返す．
  int
  fault_num() const;

  /// @brief このノードに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号2をセットする．
  /// @param[in] id セットする番号
  ///
  /// 出力ノード以外では無効
  virtual
  void
  set_output_id2(int id);

  /// @brief ファンアウトを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  /// @param[in] fo_node ファンアウト先のノード
  void
  set_fanout(int pos,
	     TpgNode* fo_node);

  /// @brief immediate dominator をセットする．
  /// @param[in] dom dominator ノード
  void
  set_imm_dom(const TpgNode* dom);

  /// @brief ノード名を設定する．
  /// @param[in] name ノード名
  /// @param[in] alloc メモリアロケータ
  void
  set_name(const string& name,
	   Alloc& alloc);

  /// @brief ファンインを設定する．
  /// @param[in] inode_list ファンインのリスト
  ///
  /// と同時にファンイン用の配列も確保する．
  /// 多入力ゲートのみ意味を持つ仮想関数
  virtual
  void
  set_fanin(const vector<TpgNode*>& inode_list,
	    Alloc& alloc);

  /// @brief ファンアウト数を設定する．
  /// @param[in] fanout_num
  /// @param[in] alloc メモリアロケータ
  ///
  /// 同時にファンアウト用の配列も確保する．
  void
  set_fanout_num(int fanout_num,
		 Alloc& alloc);

  /// @brief 故障リストを設定する．
  void
  set_fault_list(const vector<TpgFault*>& fault_list,
		 Alloc& alloc);

  /// @brief MFFC を設定する．
  /// @param[in] mffc このノードを根とするMFFC
  void
  set_mffc(const TpgMFFC* mffc);

  /// @brief FFR を設定する．
  /// @param[in] ffr このノードを根とするFFR
  void
  set_ffr(TpgFFR* ffr);

  /// @brief このノードが持っている代表故障をリストに追加する．
  void
  add_to_fault_list(vector<TpgFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DFS を行い FFR 内のノードと故障を求める．
  /// @param[out] fault_list 故障のリスト
  void
  dfs_ffr(vector<TpgFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  int mId;

  // ノード名
  char* mName;

  // ファンアウト数
  int mFanoutNum;

  // ファンアウトの配列
  TpgNode** mFanoutList;

  // immediate dominator
  const TpgNode* mImmDom;

  // FFR
  const TpgFFR* mFfr;

  // MFFC
  const TpgMFFC* mMffc;

  // 故障数
  int mFaultNum;

  // 故障のリスト
  TpgFault** mFaultList;

};

/// @relates TpgNode
/// @brief ノード名を出力する
/// @param[in] s 出力先のストリーム
/// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNode* node);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を得る．
inline
int
TpgNode::id() const
{
  return mId;
}

// @brief ノード名を返す．
inline
const char*
TpgNode::name() const
{
  return mName;
}

// @brief ファンアウト数を得る．
inline
int
TpgNode::fanout_num() const
{
  return mFanoutNum;
}

// @brief ファンアウトを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
inline
TpgNode*
TpgNode::fanout(int pos) const
{
  ASSERT_COND( pos < fanout_num() );
  return mFanoutList[pos];
}

// @brief FFR の根のノードを得る．
//
// 自分が根の場合には自分自身を返す．
inline
const TpgNode*
TpgNode::ffr_root() const
{
  if ( fanout_num() == 0 || fanout_num() > 1 ) {
    return this;
  }
  return fanout(0)->ffr_root();
}

// @brief FFRの根の場合にFFRを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgFFR*
TpgNode::ffr() const
{
  return mFfr;
}

// @brief MFFCの根のノードを得る．
//
// 自分が根の場合には自分自身を返す．
inline
const TpgNode*
TpgNode::mffc_root() const
{
  if ( imm_dom() == nullptr ) {
    return this;
  }
  return imm_dom()->mffc_root();
}

// @brief MFFCの根の場合にMFFCを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgMFFC*
TpgNode::mffc() const
{
  return mMffc;
}

// @brief 直近の dominator を得る．
inline
const TpgNode*
TpgNode::imm_dom() const
{
  return mImmDom;
}

// @brief このノードに含まれる代表故障の数を返す．
inline
int
TpgNode::fault_num() const
{
  return mFaultNum;
}

// @brief このノードに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
TpgNode::fault(int pos) const
{
  ASSERT_COND( pos < fault_num() );

  return mFaultList[pos];
}

END_NAMESPACE_YM_SATPG

#endif // TPGNODE_H
