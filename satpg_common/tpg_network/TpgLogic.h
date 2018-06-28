﻿#ifndef TPGLOGIC_H
#define TPGLOGIC_H

/// @file TpgLogic.h
/// @brief TpgLogic のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic :
  public TpgNode
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogic(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogic();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief logic タイプの時 true を返す．
  virtual
  bool
  is_logic() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 0入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic0 :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogic0(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogic0();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 1入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic1 :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin ファンイン
  TpgLogic1(int id,
	    TpgNode* fanin);

  /// @brief デストラクタ
  virtual
  ~TpgLogic1();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン
  const TpgNode* mFanin;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 2入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic2 :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 2 であることを仮定している．
  TpgLogic2(int id,
	    const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogic2();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  const TpgNode* mFanins[2];

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 3入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic3 :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 3 であることを仮定している．
  TpgLogic3(int id,
	    const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogic3();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  const TpgNode* mFanins[3];

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 4入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic4 :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] fanin_list ファンインのリスト
  ///
  /// fanin_list.size() == 4 であることを仮定している．
  TpgLogic4(int id,
	    const vector<TpgNode*>& fanin_list);

  /// @brief デストラクタ
  virtual
  ~TpgLogic4();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  const TpgNode* mFanins[4];

};


//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief n入力の論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicN :
  public TpgLogic
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogicN(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogicN();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

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
  const TpgNode*
  fanin(int pos) const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインを設定する．
  /// @param[in] inode_list ファンインのリスト
  ///
  /// と同時にファンイン用の配列も確保する．
  /// 多入力ゲートのみ意味を持つ仮想関数
  virtual
  void
  set_fanin(const vector<TpgNode*>& inode_list,
	    Alloc& alloc) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン数
  int mFaninNum;

  // ファンインの配列
  const TpgNode** mFanins;

};

END_NAMESPACE_YM_SATPG

#endif // TPGLOGIC_H
