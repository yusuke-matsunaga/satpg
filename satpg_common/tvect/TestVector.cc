
/// @file TestVector.cc
/// @brief TestVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TestVector.h"
#include "NodeValList.h"
#include "TpgDff.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] input_vector入力のベクタ
// @param[in] dff_vector DFFのベクタ
// @param[in] aux_input_vector ２時刻目の入力のベクタ
TestVector::TestVector(InputVector* input_vector,
		       DffVector* dff_vector,
		       InputVector* aux_input_vector) :
  mInputVector(input_vector),
  mDffVector(dff_vector),
  mAuxInputVector(aux_input_vector)
{
}

// @brief デストラクタ
TestVector::~TestVector()
{
}

// @brief X の個数を得る．
ymuint
TestVector::x_count() const
{
  ymuint ans = mInputVector->x_count();
  if ( mDffVector != nullptr ) {
    ans += mDffVector->x_count();
  }
  if ( mAuxInputVector != nullptr ) {
    ans += mAuxInputVector->x_count();
  }
  return ans;
}

// @brief 2つのベクタが両立しないとき true を返す．
bool
TestVector::is_conflict(const TestVector& tv1,
			const TestVector& tv2)
{
  ASSERT_COND( tv1.input_num() == tv2.input_num() );
  ASSERT_COND( tv1.dff_num() == tv2.dff_num() );
  ASSERT_COND( tv1.has_aux_input() == tv2.has_aux_input() );

  if ( BitVector::is_conflict(tv1.input_vector(), tv2.input_vector()) ) {
    return true;
  }
  if ( tv1.dff_num() > 0 ) {
    if ( BitVector::is_conflict(tv1.dff_vector(), tv2.dff_vector()) ) {
      return true;
    }
  }
  if ( tv1.has_aux_input() ) {
    if ( BitVector::is_conflict(tv1.aux_input_vector(), tv2.aux_input_vector()) ) {
      return true;
    }
  }
  return false;
}

// @brief 等価関係の比較を行なう．
// @param[in] right オペランド
// @return 自分自身と right が等しいとき true を返す．
bool
TestVector::operator==(const TestVector& right) const
{
  ASSERT_COND( input_num() == right.input_num() );
  ASSERT_COND( dff_num() == right.dff_num() );
  ASSERT_COND( has_aux_input() == right.has_aux_input() );

  if ( input_vector() != right.input_vector() ) {
    return false;
  }
  if ( dff_num() > 0 ) {
    if ( dff_vector() != right.dff_vector() ) {
      return false;
    }
  }
  if ( has_aux_input() ) {
    if ( aux_input_vector() != right.aux_input_vector() ) {
      return false;
    }
  }
  return true;
}

// @brief 包含関係の比較を行なう
// @param[in] right オペランド
// @return minterm の集合として right が自分自身を含んでいたら true を返す．
// @note false だからといって逆に自分自身が right を含むとは限らない．
bool
TestVector::operator<(const TestVector& right) const
{
  ASSERT_COND( input_num() == right.input_num() );
  ASSERT_COND( dff_num() == right.dff_num() );
  ASSERT_COND( has_aux_input() == right.has_aux_input() );

  if ( !(input_vector() < right.input_vector()) ) {
    return false;
  }
  if ( dff_num() > 0 ) {
    if ( !(dff_vector() < right.dff_vector()) ) {
      return false;
    }
  }
  if ( has_aux_input() ) {
    if ( !(aux_input_vector() < right.aux_input_vector()) ) {
      return false;
    }
  }
  return true;
}

// @brief 包含関係の比較を行なう
// @param[in] right オペランド
// @return minterm の集合として right が自分自身を含んでいたら true を返す．
// @note こちらは等しい場合も含む．
// @note false だからといって逆に自分自身が right を含むとは限らない．
bool
TestVector::operator<=(const TestVector& right) const
{
  ASSERT_COND( input_num() == right.input_num() );
  ASSERT_COND( dff_num() == right.dff_num() );
  ASSERT_COND( has_aux_input() == right.has_aux_input() );

  if ( !(input_vector() <= right.input_vector()) ) {
    return false;
  }
  if ( dff_num() > 0 ) {
    if ( !(dff_vector() <= right.dff_vector()) ) {
      return false;
    }
  }
  if ( has_aux_input() ) {
    if ( !(aux_input_vector() <= right.aux_input_vector()) ) {
      return false;
    }
  }
  return true;
}

// @brief すべて未定(X) で初期化する．
void
TestVector::init()
{
  mInputVector->init();
  if ( mDffVector != nullptr ) {
    mDffVector->init();
  }
  if ( mAuxInputVector != nullptr ) {
    mAuxInputVector->init();
  }
}

// @brief InputVector とDffVector から値を設定する．
// @param[in] input_vector入力のベクタ
// @param[in] dff_vector DFFのベクタ
// @param[in] aux_input_vector ２時刻目の入力のベクタ
void
TestVector::set(const InputVector& input_vector,
		const DffVector& dff_vector,
		const InputVector& aux_input_vector)
{

  mInputVector->copy(input_vector);
  mDffVector->copy(dff_vector);
  mAuxInputVector->copy(aux_input_vector);
}

// @brief 割当リストから内容を設定する．
// @param[in] assign_list 割当リスト
//
// assign_list に外部入力以外の割当が含まれている場合無視する．
void
TestVector::set_from_assign_list(const NodeValList& assign_list)
{
  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    Val3 val = nv.val() ? kVal1 : kVal0;
    const TpgNode* node = nv.node();
    ASSERT_COND( node->is_ppi() );
    if ( fault_type() == kFtStuckAt ) {
      ASSERT_COND( nv.time() == 1 );

      ymuint id = node->input_id();
      set_ppi_val(id, val);
    }
    else {
      if ( node->is_primary_input() ) {
	ymuint id = node->input_id();
	if ( nv.time() == 1 ) {
	  set_aux_input_val(id, val);
	}
	else {
	  set_input_val(id, val);
	}
      }
      else if ( node->is_dff_output() ) {
	ASSERT_COND( nv.time() == 0 );

	ymuint id = node->dff()->id();
	set_dff_val(id, val);
      }
    }
  }
}

// @brief HEX文字列から内容を設定する．
// @param[in] hex_string HEX 文字列
// @return hex_string に不適切な文字が含まれていたら false を返す．
// @note hex_string が短い時には残りは0で初期化される．
// @note hex_string が長い時には余りは捨てられる．
bool
TestVector::set_from_hex(const string& hex_string)
{
  // C++ はこういう文字列処理がめんどくさい．
  string tmp_string(hex_string);
  if ( mDffVector != nullptr ) {
    size_t pos1 = tmp_string.find(":");
    if ( pos1 == string::npos ) {
      return false;
    }
    if ( mAuxInputVector != nullptr ) {
      size_t pos2 = tmp_string.find(":", pos1 + 1);
      if ( pos2 == string::npos ) {
	return false;
      }
      string aux_input_str = tmp_string.substr(pos2 + 1, string::npos);
      bool stat1 = mAuxInputVector->set_from_hex(aux_input_str);
      if ( !stat1 ) {
	return false;
      }
      tmp_string = tmp_string.substr(0, pos2);
    }
    string dff_str = tmp_string.substr(pos1 + 1, string::npos);
    bool stat2 = mDffVector->set_from_hex(dff_str);
    if ( !stat2 ) {
      return false;
    }
    tmp_string = tmp_string.substr(0, pos1);
  }
  bool stat3 = mInputVector->set_from_hex(tmp_string);

  return stat3;
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
void
TestVector::set_from_random(RandGen& randgen)
{
  mInputVector->set_from_random(randgen);
  if ( mDffVector != nullptr ) {
    mDffVector->set_from_random(randgen);
  }
  if ( mAuxInputVector != nullptr ) {
    mAuxInputVector->set_from_random(randgen);
  }
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
void
TestVector::fix_x_from_random(RandGen& randgen)
{
  mInputVector->fix_x_from_random(randgen);
  if ( mDffVector != nullptr ) {
    mDffVector->fix_x_from_random(randgen);
  }
  if ( mAuxInputVector != nullptr ) {
    mAuxInputVector->fix_x_from_random(randgen);
  }
}

// @brief テストベクタをコピーする．
// @param[in] src コピー元のテストベクタ
// @note X の部分はコピーしない．
void
TestVector::copy(const TestVector& src)
{
  ASSERT_COND( input_num() == src.input_num() );
  ASSERT_COND( dff_num() == src.dff_num() );
  ASSERT_COND( has_aux_input() == src.has_aux_input() );

  mInputVector->copy(src.input_vector());
  if ( mDffVector != nullptr ) {
    mDffVector->copy(src.dff_vector());
  }
  if ( mAuxInputVector != nullptr ) {
    mAuxInputVector->copy(src.aux_input_vector());
  }
}

// @breif テストベクタをマージする．
// @note X 以外で相異なるビットがあったら false を返す．
bool
TestVector::merge(const TestVector& src)
{
  ASSERT_COND( input_num() == src.input_num() );
  ASSERT_COND( dff_num() == src.dff_num() );
  ASSERT_COND( has_aux_input() == src.has_aux_input() );

  bool stat1 = mInputVector->merge(src.input_vector());
  if ( !stat1 ) {
    return false;
  }

  if ( mDffVector != nullptr ) {
    bool stat2 = mDffVector->merge(src.dff_vector());
    if ( !stat2 ) {
      return false;
    }
  }
  if ( mAuxInputVector != nullptr ) {
    bool stat3 = mAuxInputVector->merge(src.aux_input_vector());
    if ( !stat3 ) {
      return false;
    }
  }

  return true;
}

// @brief 内容を BIN 形式で表す．
string
TestVector::bin_str() const
{
  string ans = mInputVector->bin_str();
  if ( mDffVector != nullptr ) {
    ans += ":" + mDffVector->bin_str();
    if ( mAuxInputVector != nullptr ) {
      ans += ":" + mAuxInputVector->bin_str();
    }
  }
  return ans;
}

// @brief 内容を HEX 形式で出力する．
string
TestVector::hex_str() const
{
  string ans = mInputVector->hex_str();
  if ( mDffVector != nullptr ) {
    ans += ":" + mDffVector->hex_str();
    if ( mAuxInputVector != nullptr ) {
      ans += ":" + mAuxInputVector->hex_str();
    }
  }
  return ans;
}

END_NAMESPACE_YM_SATPG
