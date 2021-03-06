#ifndef TPGPPI_H
#define TPGPPI_H

/// @file TpgPPI.h
/// @brief TpgPPI のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"


BEGIN_NAMESPACE_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgPPI TpgPPI.h "TpgPPI.h"
/// @brief FF出力も含めた擬似外部入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgPPI :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] input_id 入力番号
  TpgPPI(int id,
	 int input_id);

  /// @brief デストラクタ
  virtual
  ~TpgPPI();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  virtual
  bool
  is_ppi() const override;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::input(node->input_id()
  /// の関係を満たす．
  /// is_input() が false の場合の返り値は不定
  virtual
  int
  input_id() const override;

  /// @brief ゲートタイプを得る．
  ///
  /// - is_logic() が true の時はゲートタイプを返す．
  /// - is_ppi() が true の時は kGateINPUT を返す．
  /// - is_ppo() が true の時は kGateBUFF を返す．
  /// - それ以外の返り値は不定
  virtual
  GateType
  gate_type() const override;

  /// @brief ファンインのリストを得る．
  virtual
  Array<const TpgNode*>
  fanin_list() const override;

  /// @brief ファンイン数を得る．
  virtual
  int
  fanin_num() const override;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力番号
  int mInputId;

};

END_NAMESPACE_SATPG

#endif // TPGPPI_H
