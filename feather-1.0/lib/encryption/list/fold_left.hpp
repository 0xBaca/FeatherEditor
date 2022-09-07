# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_LIST_FOLD_LEFT_HPP
# define BOOST_PREPROCESSOR_LIST_FOLD_LEFT_HPP
#
# include "cat.hpp"
# include "control/while.hpp"
# include "debug/error.hpp"
# include "detail/auto_rec.hpp"
#
# /* BOOST_PP_LIST_FOLD_LEFT */
#
# if 0
#    define BOOST_PP_LIST_FOLD_LEFT(op, state, list)
# endif
#
# define BOOST_PP_LIST_FOLD_LEFT BOOST_PP_CAT(BOOST_PP_LIST_FOLD_LEFT_, BOOST_PP_AUTO_REC(BOOST_PP_WHILE_P, 256))
#
# define BOOST_PP_LIST_FOLD_LEFT_257(o, s, l) BOOST_PP_ERROR(0x0004)
#
# define BOOST_PP_LIST_FOLD_LEFT_D(d, o, s, l) BOOST_PP_LIST_FOLD_LEFT_ ## d(o, s, l)
# define BOOST_PP_LIST_FOLD_LEFT_2ND BOOST_PP_LIST_FOLD_LEFT
# define BOOST_PP_LIST_FOLD_LEFT_2ND_D BOOST_PP_LIST_FOLD_LEFT_D
#
# if BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    include <boost/preprocessor/list/detail/edg/fold_left.hpp"
# elif BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_DMC()
#    include <boost/preprocessor/list/detail/dmc/fold_left.hpp"
# else
#    include <boost/preprocessor/list/detail/fold_left.hpp"
# endif
#
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_NIL 1
#
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_1(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_2(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_3(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_4(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_5(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_6(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_7(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_8(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_9(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_10(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_11(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_12(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_13(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_14(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_15(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_16(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_17(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_18(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_19(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_20(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_21(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_22(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_23(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_24(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_25(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_26(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_27(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_28(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_29(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_30(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_31(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_32(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_33(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_34(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_35(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_36(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_37(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_38(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_39(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_40(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_41(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_42(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_43(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_44(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_45(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_46(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_47(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_48(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_49(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_50(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_51(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_52(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_53(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_54(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_55(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_56(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_57(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_58(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_59(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_60(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_61(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_62(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_63(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_64(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_65(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_66(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_67(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_68(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_69(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_70(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_71(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_72(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_73(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_74(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_75(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_76(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_77(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_78(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_79(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_80(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_81(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_82(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_83(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_84(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_85(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_86(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_87(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_88(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_89(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_90(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_91(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_92(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_93(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_94(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_95(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_96(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_97(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_98(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_99(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_100(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_101(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_102(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_103(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_104(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_105(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_106(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_107(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_108(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_109(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_110(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_111(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_112(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_113(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_114(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_115(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_116(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_117(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_118(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_119(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_120(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_121(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_122(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_123(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_124(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_125(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_126(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_127(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_128(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_129(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_130(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_131(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_132(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_133(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_134(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_135(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_136(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_137(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_138(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_139(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_140(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_141(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_142(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_143(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_144(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_145(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_146(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_147(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_148(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_149(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_150(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_151(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_152(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_153(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_154(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_155(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_156(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_157(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_158(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_159(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_160(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_161(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_162(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_163(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_164(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_165(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_166(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_167(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_168(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_169(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_170(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_171(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_172(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_173(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_174(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_175(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_176(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_177(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_178(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_179(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_180(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_181(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_182(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_183(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_184(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_185(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_186(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_187(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_188(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_189(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_190(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_191(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_192(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_193(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_194(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_195(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_196(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_197(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_198(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_199(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_200(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_201(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_202(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_203(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_204(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_205(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_206(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_207(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_208(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_209(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_210(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_211(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_212(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_213(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_214(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_215(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_216(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_217(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_218(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_219(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_220(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_221(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_222(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_223(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_224(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_225(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_226(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_227(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_228(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_229(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_230(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_231(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_232(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_233(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_234(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_235(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_236(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_237(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_238(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_239(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_240(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_241(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_242(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_243(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_244(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_245(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_246(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_247(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_248(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_249(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_250(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_251(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_252(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_253(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_254(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_255(o, s, l) 0
# define BOOST_PP_LIST_FOLD_LEFT_CHECK_BOOST_PP_LIST_FOLD_LEFT_256(o, s, l) 0
#
# endif
