#ifndef SNGATE_H
#define SNGATE_H

/// @file SnGate.h
/// @brief SnGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SimNode.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnGate SimNode.h
/// @brief 多入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate(ymuint id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnGate();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  ymuint
  _fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  SimNode*
  _fanin(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  ymuint mFaninNum;

  // ファンインの配列
  SimNode** mFanins;

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate1 SimNode.h
/// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate1 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate1(ymuint id,
	  const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnGate1();


public:

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを得る．
  SimNode*
  _fanin() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン
  SimNode* mFanin;

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate2 SimNode.h
/// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate2 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate2(ymuint id,
	  const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnGate2();


public:

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインを得る．
  SimNode*
  _fanin(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SimNode* mFanins[2];

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate3 SimNode.h
/// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate3 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate3(ymuint id,
	  const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnGate3();


public:

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインを得る．
  SimNode*
  _fanin(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SimNode* mFanins[3];

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate4 SimNode.h
/// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate4 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate4(ymuint id,
	  const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnGate4();


public:

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインを得る．
  SimNode*
  _fanin(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SimNode* mFanins[4];

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ファンイン数を得る．
inline
ymuint
SnGate::_fanin_num() const
{
  return mFaninNum;
}

// @brief pos 番めのファンインを得る．
inline
SimNode*
SnGate::_fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief ファンインを得る．
inline
SimNode*
SnGate1::_fanin() const
{
  return mFanin;
}

// @brief pos 番めのファンインを得る．
inline
SimNode*
SnGate2::_fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief pos 番めのファンインを得る．
inline
SimNode*
SnGate3::_fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief pos 番めのファンインを得る．
inline
SimNode*
SnGate4::_fanin(ymuint pos) const
{
  return mFanins[pos];
}

END_NAMESPACE_YM_SATPG_FSIM

#endif // SNGATE_H
