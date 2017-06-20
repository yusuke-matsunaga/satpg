#ifndef SNOR_H
#define SNOR_H

/// @file SnOr.h
/// @brief SnOr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "SnGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnOr SimNode.h
/// @brief ORノード
//////////////////////////////////////////////////////////////////////
class SnOr :
  public SnGate
{
public:

  /// @brief コンストラクタ
  SnOr(ymuint id,
       const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr2 SimNode.h
/// @brief 2入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr2 :
  public SnGate2
{
public:

  /// @brief コンストラクタ
  SnOr2(ymuint id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr2();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr3 SimNode.h
/// @brief 3入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr3 :
  public SnGate3
{
public:

  /// @brief コンストラクタ
  SnOr3(ymuint id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr3();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr4 SimNode.h
/// @brief 4入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr4 :
  public SnGate4
{
public:

  /// @brief コンストラクタ
  SnOr4(ymuint id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr4();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor SimNode.h
/// @brief NORノード
//////////////////////////////////////////////////////////////////////
class SnNor :
  public SnOr
{
public:

  /// @brief コンストラクタ
  SnNor(ymuint id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor2 SimNode.h
/// @brief 2入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor2 :
  public SnOr2
{
public:

  /// @brief コンストラクタ
  SnNor2(ymuint id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor2();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor3 SimNode.h
/// @brief 3入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor3 :
  public SnOr3
{
public:

  /// @brief コンストラクタ
  SnNor3(ymuint id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor3();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor2 SimNode.h
/// @brief 2入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor4 :
  public SnOr4
{
public:

  /// @brief コンストラクタ
  SnNor4(ymuint id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor4();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val();

};

END_NAMESPACE_YM_SATPG_FSIM

#endif // SNOR_H
