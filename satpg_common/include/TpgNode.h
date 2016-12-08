﻿#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "LitMap.h"
#include "Val3.h"
#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgNode TpgNode.h "TpgNode.h"
/// @brief SATPG 用のノードを表すクラス
///
/// 基本的には一つの BnNode に対応しているが，
/// もとのゲートが組み込み型でない場合には複数の TpgNode を組み合わ
/// せてもとのゲートを表す．
/// そのため，場合によってはファンインの故障を表すための仮想的な
/// ノードを挿入する場合もある．
///
/// 大規模な回路の場合，データ構造をコンパクトにすることが高速化に
/// つながるのでちょっとトリッキーなことをしている．
/// 具体的には TpgNode のインスタンスを生成するときに，ファンイン数と
/// ファンアウト数を調べ，その分だけ TpgNode の後ろの領域に
/// TpgNode* の配列分だけ余計に確保する．
/// こうすることで mNodeList[] があたかも固定の配列のように
/// (つまり余分な間接参照なしに)アクセスすることができる．
/// そのため通常のコンストラクタを使うことができないため，
/// make_XXXX() の形のクラスメソッドを使う．
///
/// また，ファンインとファンアウトは一つの配列に続けて格納する．
/// ファンインは 0 〜 (mFaninNum - 1) まで
/// ファンアウトは mFaninNum 〜 (mFaninNum + mFanoutNum - 1)
/// となっている．
//////////////////////////////////////////////////////////////////////
class TpgNode
{
public:

  /// @brief 入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] iid 入力番号
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_input(ymuint id,
	     ymuint iid,
	     ymuint fanout_num);

  /// @brief 出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] oid 出力番号
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_output(ymuint id,
	      ymuint oid,
	      TpgNode* inode);

  /// @brief DFFの入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] oid 出力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_input(ymuint id,
		 ymuint oid,
		 TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFの出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] iid 入力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_output(ymuint id,
		  ymuint iid,
		  TpgDff* dff,
		  ymuint fanout_num);

  /// @brief DFFのクロック端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_clock(ymuint id,
		 TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFのクリア端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_clear(ymuint id,
		 TpgDff* dff,
		 TpgNode* inode);

  /// @brief DFFのプリセット端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_preset(ymuint id,
		  TpgDff* dff,
		  TpgNode* inode);

  /// @brief 論理ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] gate_type ゲートタイプ
  /// @param[in] inode_list 入力ノードのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_logic(ymuint id,
	     GateType gate_type,
	     const vector<TpgNode*>& inode_list,
	     ymuint fanout_num);


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  /// @param[in] fanout_num ファンアウト数
  TpgNode(ymuint id,
	  const vector<TpgNode*>& fanin_list,
	  ymuint fanout_num);

  /// @brief デストラクタ
  ~TpgNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  ymuint
  id() const;

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
  /// node = TpgNetwork::input(node->input_id())
  /// の関係を満たす．
  /// is_input() が false の場合の返り値は不定
  virtual
  ymuint
  input_id() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::output(node->output_id())
  /// の関係を満たす．
  /// is_output() が false の場合の返り値は不定
  virtual
  ymuint
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  virtual
  ymuint
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
  /// is_logic() が false の場合の返り値は不定
  virtual
  GateType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  noval() const;

  /// @brief ファンイン数を得る．
  ymuint
  fanin_num() const;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  TpgNode*
  fanin(ymuint pos) const;

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief ファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  TpgNode*
  fanout(ymuint pos) const;

  /// @brief FFR の根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  ffr_root() const;

  /// @brief 直近の dominator を得る．
  const TpgNode*
  imm_dom() const;

  /// @brief MFFC の根の場合に MFFC内の根のノード数を返す．
  ymuint
  mffc_elem_num() const;

  /// @brief MFFC の根の場合に MFFC内の根のノードを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
  ///
  /// pos == 0 の時は常に自分を返す．
  const TpgNode*
  mffc_elem(ymuint pos) const;


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
  set_output_id2(ymuint id);

  /// @brief ファンアウトを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  /// @param[in] fo_node ファンアウト先のノード
  void
  set_fanout(ymuint pos,
	     TpgNode* fo_node);

  /// @brief immediate dominator をセットする．
  /// @param[in] dom dominator ノード
  void
  set_imm_dom(const TpgNode* dom);

  /// @brief MFFC 内の根のノードの情報をセットする．
  /// @param[in] num 要素数
  /// @param[in] node_list ノードのリスト
  void
  set_mffc_info(ymuint num,
		TpgNode** node_list);


public:
  //////////////////////////////////////////////////////////////////////
  // CNF の生成に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入出力の関係を表す CNF 式を生成する．
  /// @param[in] solver SAT ソルバ
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  virtual
  void
  make_cnf(SatSolver& solver,
	   const LitMap& lit_map) const;

  /// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
  /// @param[in] solver SAT ソルバ
  /// @param[in] fpos 故障のある入力位置
  /// @param[in] fval 故障値 ( 0 / 1 )
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  ///
  /// こちらは入力に故障を仮定したバージョン
  virtual
  void
  make_faulty_cnf(SatSolver& solver,
		  ymuint fpos,
		  int fval,
		  const LitMap& lit_map) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // ファンイン数
  ymuint32 mFaninNum;

  // ファンアウト数
  ymuint32 mFanoutNum;

  // MFFC 内の根のノード数
  ymuint32 mMffcElemNum;

  // immediate dominator
  const TpgNode* mImmDom;

  // MFFC 内の根のノードのリスト
  TpgNode** mMffcElemList;

  // ファンインの配列
  TpgNode** mFaninList;

  // ファンアウトの配列
  TpgNode** mFanoutList;

};

/// @relates TpgNode
/// @brief ノード名を出力する
/// @param[in] s 出力先のストリーム
/// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNetwork& network,
	   const TpgNode* node);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を得る．
inline
ymuint
TpgNode::id() const
{
  return mId;
}

// @brief ファンイン数を得る．
inline
ymuint
TpgNode::fanin_num() const
{
  return mFaninNum;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
inline
TpgNode*
TpgNode::fanin(ymuint pos) const
{
  ASSERT_COND( pos < fanin_num() );
  return mFaninList[pos];
}

// @brief ファンアウト数を得る．
inline
ymuint
TpgNode::fanout_num() const
{
  return mFanoutNum;
}

// @brief ファンアウトを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
inline
TpgNode*
TpgNode::fanout(ymuint pos) const
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

// @brief 直近の dominator を得る．
inline
const TpgNode*
TpgNode::imm_dom() const
{
  return mImmDom;
}

// @brief MFFC の根の場合に MFFC内の根のノード数を返す．
inline
ymuint
TpgNode::mffc_elem_num() const
{
  return mMffcElemNum;
}

// @brief MFFC の根の場合に MFFC内の根のノードを返す．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
//
// pos == 0 の時は常に自分を返す．
inline
const TpgNode*
TpgNode::mffc_elem(ymuint pos) const
{
  ASSERT_COND( pos < mffc_elem_num() );
  return mMffcElemList[pos];
}

END_NAMESPACE_YM_SATPG

#endif // TPGNODE_H