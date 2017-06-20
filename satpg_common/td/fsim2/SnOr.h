#ifndef SNOR_H
#define SNOR_H

/// @file SnOr.h
/// @brief SnOr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SnGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

//////////////////////////////////////////////////////////////////////
/// @class SnOr SimNode.h
/// @brief ORノード
//////////////////////////////////////////////////////////////////////
class SnOr :
  public SnGate
{
public:

  /// @brief コンストラクタ
  SnOr(ymuint32 id,
       const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  calc_gobs(ymuint ipos);

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnOr2(ymuint32 id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr2();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  calc_gobs(ymuint ipos);

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnOr3(ymuint32 id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr3();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  calc_gobs(ymuint ipos);

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnOr4(ymuint32 id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnOr4();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  calc_gobs(ymuint ipos);

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnNor(ymuint32 id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnNor2(ymuint32 id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor2();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnNor3(ymuint32 id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor3();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

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
  SnNor4(ymuint32 id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNor4();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 1時刻前の正常値の計算を行う．
  virtual
  PackedVal
  _calc_hval();

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

};

END_NAMESPACE_YM_SATPG_FSIM2

#endif // SNOR_H
