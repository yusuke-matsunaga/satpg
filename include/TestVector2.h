﻿#ifndef TESTVECTOR2_H
#define TESTVECTOR2_H

/// @file TestVector2t.h
/// @brief TestVector2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "Val3.h"
#include "PackedVal.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TestVector2 TestVector2.h "TestVector2.h"
/// @brief テストベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
/// 生成/破壊は Tv2Mgr のみが行う．
/// 同じ Tv2Mgr が扱うテストベクタのサイズはすべて同じ．
///
/// こちらはブロードサイド方式の遷移故障用ベクタなので
/// 2時刻分のベクタを持つ．
//////////////////////////////////////////////////////////////////////
class TestVector2
{
  friend class Tv2Mgr;

public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を得る．
  ///
  /// FF数も含む．
  ymuint
  input_num() const;

  /// @brief 1時刻前の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  Val3
  prev_val3(ymuint pos) const;

  /// @brief 外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  Val3
  cur_val3(ymuint pos) const;

  /// @brief X の個数を得る．
  ymuint
  x_num() const;

  /// @brief 2つのベクタが両立しないとき true を返す．
  /// @param[in] tv1, tv2 対象のテストベクタ
  ///
  /// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
  static
  bool
  is_conflict(const TestVector2& tv1,
	      const TestVector2& tv2);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] right オペランド
  /// @return 自分自身と right が等しいとき true を返す．
  bool
  operator==(const TestVector2& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  /// @note false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<(const TestVector2& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  /// @note こちらは等しい場合も含む．
  /// @note false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<=(const TestVector2& right) const;

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @breif 1時刻前の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  void
  set_prev_val(ymuint pos,
	       Val3 val);

  /// @breif 外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  void
  set_cur_val(ymuint pos,
	      Val3 val);

  /// @brief 割当リストから内容を設定する．
  /// @param[in] assign_list 割当リスト
  ///
  /// assign_list に外部入力以外の割当が含まれている場合無視する．
  void
  set_from_assign_list(const NodeVal2List& assign_list);

  /// @brief HEX文字列から内容を設定する．
  /// @param[in] hex_string HEX 文字列
  /// @return hex_string に不適切な文字が含まれていたら false を返す．
  /// @note hex_string が短い時には残りは0で初期化される．
  /// @note hex_string が長い時には余りは捨てられる．
  bool
  set_from_hex(const string& hex_string);

  /// @brief 乱数パタンを設定する．
  /// @param[in] randgen 乱数生成器
  /// @note 結果はかならず 0 か 1 になる．(Xは含まれない)
  void
  set_from_random(RandGen& randgen);

  /// @brief X の部分を乱数で 0/1 に設定する．
  /// @param[in] randgen 乱数生成器
  void
  fix_x_from_random(RandGen& randgen);

  /// @brief テストベクタをコピーする．
  /// @param[in] src コピー元のテストベクタ
  /// @note X の部分はコピーしない．
  void
  copy(const TestVector2& src);

  /// @breif テストベクタをマージする．
  /// @note X 以外で相異なるビットがあったら false を返す．
  bool
  merge(const TestVector2& src);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  ymuint
  _vect_len() const;

  /// @brief pos 番めの値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  Val3
  _val3(ymuint pos) const;

  /// @breif pos 番めの値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  void
  _set_val3(ymuint pos,
	    Val3 val);

  /// @brief ブロック数を返す．
  /// @param[in] ni 入力数
  static
  ymuint
  block_num(ymuint ni);

  /// @brief HEX文字列の長さを返す．
  /// @param[in] ni 入力数
  static
  ymuint
  hex_length(ymuint ni);

  // 入力位置からブロック番号を得る．
  /// @param[in] ipos 入力の位置番号
  static
  ymuint
  block_idx(ymuint ipos);

  // 入力位置からシフト量を得る．
  /// @param[in] ipos 入力の位置番号
  static
  ymuint
  shift_num(ymuint ipos);


private:
  //////////////////////////////////////////////////////////////////////
  // 特殊なアロケーションをしているのでコンストラクタ関係は
  // プライベートにしている．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] input_num 入力数
  explicit
  TestVector2(ymuint input_num);

  /// @brief デストラクタ
  ~TestVector2();

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  TestVector2(const TestVector2& src);

  /// @brief 代入演算子
  /// @param[in] src コピー元のソース
  const TestVector2&
  operator=(const TestVector2& src);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数(FF数も含む)
  ymuint mInputNum;

  // 最後のブロックのマスク
  PackedVal mMask;

  // ベクタ本体(ただしサイズは可変)
  PackedVal mPat[1];


private:
  //////////////////////////////////////////////////////////////////////
  // このクラスに固有の定数
  //////////////////////////////////////////////////////////////////////

  // 1ワードあたりのHEX文字数
  static
  const ymuint32 HPW = kPvBitLen / 4;

};

/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const TestVector2& left,
	   const TestVector2& right)
{
  return !left.operator==(right);
}

/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(const TestVector2& left,
	  const TestVector2& right)
{
  return right.operator<(left);
}

/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note こちらは等しい場合も含む．
/// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>=(const TestVector2& left,
	   const TestVector2& right)
{
  return right.operator<=(left);
}


/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] tv テストベクタ
ostream&
operator<<(ostream& s,
	   const TestVector2& tv);

/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] tvp テストベクタへのポインタ
ostream&
operator<<(ostream& s,
	   const TestVector2* tvp);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 入力数を得る．
inline
ymuint
TestVector2::input_num() const
{
  return mInputNum;
}

// @brief 1時刻前の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector2::prev_val3(ymuint pos) const
{
  return _val3(pos);
}

// @brief 外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector2::cur_val3(ymuint pos) const
{
  return _val3(pos + input_num());
}

// @breif 1時刻前の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector2::set_prev_val(ymuint pos,
			  Val3 val)
{
  _set_val3(pos, val);
}

// @breif 外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector2::set_cur_val(ymuint pos,
			 Val3 val)
{
  _set_val3(pos + input_num(), val);
}

// @brief ベクタ長を返す．
inline
ymuint
TestVector2::_vect_len() const
{
  return input_num() + input_num();
}

// @brief pos 番めの値を得る．
inline
Val3
TestVector2::_val3(ymuint pos) const
{
  ymuint shift = shift_num(pos);
  ymuint block0 = block_idx(pos);
  ymuint block1 = block0 + 1;
  int v0 = (mPat[block0] >> shift) & 1UL;
  int v1 = (mPat[block1] >> shift) & 1UL;
  return static_cast<Val3>(v1 + v1 + v0);
}

// @breif pos 番めの値を設定する．
inline
void
TestVector2::_set_val3(ymuint pos,
		       Val3 val)
{
  ymuint shift = shift_num(pos);
  ymuint block0 = block_idx(pos);
  ymuint block1 = block0 + 1;
  PackedVal mask = 1UL << shift;
  if ( val == kVal0 ) {
    mPat[block0] |= mask;
    mPat[block1] &= ~mask;
  }
  else if ( val == kVal1 ) {
    mPat[block0] &= ~mask;
    mPat[block1] |= mask;
  }
  else { // val == kValX
    mPat[block0] &= ~mask;
    mPat[block1] &= ~mask;
  }
}

// @brief ブロック数を返す．
inline
ymuint
TestVector2::block_num(ymuint ni)
{
  return ((ni + kPvBitLen - 1) / kPvBitLen) * 2;
}

// @brief HEX文字列の長さを返す．
inline
ymuint
TestVector2::hex_length(ymuint ni)
{
  return (ni + 3) / 4;
}

// 入力位置からブロック番号を得る．
inline
ymuint
TestVector2::block_idx(ymuint ipos)
{
  return (ipos / kPvBitLen) * 2;
}

// 入力位置からシフト量を得る．
inline
ymuint
TestVector2::shift_num(ymuint ipos)
{
  return (kPvBitLen - 1 - ipos) % kPvBitLen;
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const TestVector2& tv)
{
  return s << tv.bin_str();
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const TestVector2* tvp)
{
  return s << tvp->bin_str();
}

END_NAMESPACE_YM_SATPG

#endif // TESTVECTOR2_H
