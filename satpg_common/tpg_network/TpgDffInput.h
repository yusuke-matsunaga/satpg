#ifndef TPGDFFINPUT_H
#define TPGDFFINPUT_H

/// @file TpgDffInput.h
/// @brief TpgDffInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPO.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgDffInput TpgDffInput.h "TpgDffInput.h"
/// @brief DFFの入力を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffInput :
  public TpgPPO
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] output_id 出力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanin ファンインのノード
  TpgDffInput(ymuint id,
	      ymuint output_id,
	      TpgDff* dff,
	      TpgNode* fanin);

  /// @brief デストラクタ
  ~TpgDffInput();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
  virtual
  bool
  is_dff_input() const;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  TpgDff*
  dff() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF
  TpgDff* mDff;

};

END_NAMESPACE_YM_SATPG

#endif // TPGDFFINPUT_H
