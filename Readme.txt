/* ===================================
�@�@�@�@�@�@�@Proxydomo�@�@�@�@�@�@�@�@�@�@�@�@
 ==================================== */
 
���͂��߂�
���̃\�t�g�̓��[�J���œ����v���N�V�t�B���^�����O�\�t�g�ł�
�쐬�ɂ�����I�[�v���\�[�X��Proximodo���g���č���Ă��܂��B

���g����
�N�����ā@127.0.0.1:6060(�v���N�V�|�[�g�ɕ\������Ă鐔�l)���v���N�V�Ƃ��Ďw�肷���
�v���N�V�t�B���^�Ƃ��ċ@�\���܂��B
�ڂ����g������Proxomitron�Ȃǂ��Q�l�ɂ��Ă��������B

$LST�̎w����@�� lists�t�H���_�ȉ��ɂ���e�L�X�g����g���q�����������̂ł�
����: lists\Kill.txt -> $LST(Kill)

�����m�̃o�O
�E�ꕔ�������Ă��Ȃ��R�}���h������܂�($ADDLIST�Ȃ�)

���Ɛ�
���(�����ҁ����ώ�)�́A���̃\�t�g�ɂ���Đ������@���Ȃ鑹�Q�ɂ��A
�C����X�V���A�ӔC�𕉂�Ȃ����ƂƂ��܂��B
�g�p�ɂ������ẮA���ȐӔC�ł��肢���܂��B
 
��������Ή��L��URL�ɂ��郁�[���t�H�[���ɂ��肢���܂��B
http://www31.atwiki.jp/lafe/pages/33.html
 
�����쌠�\��
Copyright (C) 2004 Antony BOUCHER
Copyright (C) 2013 amate
 
 �摜�̈ꕔ�ɁuVS2010ImageLibrary�v�̈ꕔ���g�p���Ă��܂��B
 
���r���h�ɂ���
�r���h�ɂ� boost(1.54~)�� zlib �� WTL �� ICU �� GnuTLS ���K�v�Ȃ̂ł��ꂼ��p�ӂ��Ă��������B

��boost
http://www.boost.org/

��zlib
http://zlib.net/

��ICU
http://site.icu-project.org/

��WTL
http://sourceforge.net/projects/wtl/

��GnuTLS
http://www.gnutls.org/

���R���p�C���ς�dll
zlib��GnuTLS�̃R���p�C���ς�dll�����L��URL�Ō��J���Ă��܂�
http://1drv.ms/1vqvcaG


zlib�̃w�b�_�̏ꏊ
$(SolutionDir)zlib\x86\include
$(SolutionDir)zlib\x64\include
zlib�̃��C�u�����̏ꏊ
$(SolutionDir)zlib\x86\lib
$(SolutionDir)zlib\x64\lib
���ȉ��̏ꏊ�ɂ���΂Ƃ��ɐݒ�͂���Ȃ��͂��ł�
����ȊO�̏ꏊ��zlib��u���Ă���Ȃ�K����zlibbuffer.h/cpp���C�����Ă�������

ICU ��
$(SolutionDir)icu\Win32 or Win64 �t�H���_�� include �� lib ������΃R���p�C���ʂ�悤�ɂȂ��Ă��܂�

boost::shared_mutex���g�p����̂�boost::thread�̃��C�u�������K�v�ɂȂ�܂�
 Boost���C�u�����̃r���h���@
 https://sites.google.com/site/boostjp/howtobuild
�R�}���h���C��
// x86
b2.exe install -j 2 --prefix=lib toolset=msvc-12.0 runtime-link=static --with-thread --with-date_time
// x64
b2.exe install -j 2 --prefix=lib64 toolset=msvc-12.0 runtime-link=static address-model=64 --with-thread --with-date_time


��GnuTLS (64bit��)dll�̍���

http://www.devlog.alt-area.org/?p=2802
���̃T�C�g���Q�l�� MinGW64 + MSYS �̊������

������
Strawberry Perl�̃o�[�W������"Strawberry Perl 5.18.4.1"���C���X�g�[�����Ă�������
��������
C:\Strawberry\perl\bin
�t�H���_�ɂ���"pkg-config"�A"pkg-config.bat"�t�@�C�������l�[�����Ă�������

// x64 ==============================================================
#gmp6.0.0a build
./configure --build=x86_64-w64-mingw32 --enable-shared --disable-static
make
make install

#nettle2.7.1 build
./configure --build=x86_64-w64-mingw32 --with-lib-path=/local/lib --with-include-path=/local/include
make
make install

#gnutls3.3.13 build
./configure --build=x86_64-w64-mingw32 --disable-guile --disable-nls --without-zlib PKG_CONFIG_PATH=/local/lib/pkgconfig LDFLAGS=-L/local/lib --disable-cxx --disable-openssl-compatibility --disable-doc --disable-heartbeat-support --disable-rsa-export
make
make install

// x86 =================================================================
#gmp6.0.0a build
./configure --build=i686-w64-mingw32 --enable-shared --disable-static
make
make install

#nettle2.7.1 build
./configure --build=i686-w64-mingw32 --with-lib-path=/local/lib --with-include-path=/local/include --disable-openssl --disable-documentation
make
make install

#gnutls3.3.13 build
./configure --build=i686-w64-mingw32 --disable-guile --disable-nls --without-zlib PKG_CONFIG_PATH=/local/lib/pkgconfig LDFLAGS=-L/local/lib --disable-cxx --disable-openssl-compatibility --disable-doc --disable-heartbeat-support --disable-rsa-export
make
make install

// ================================================================
msys\local
�ȉ��ɐ��������ł���

http://einguste.hatenablog.com/entry/2014/01/29/002601
lib/gnutls.pc(.in)�̏C�����K�v��������Ȃ�


"VS2013 x64 Native Tools �R�}���h �v�����v�g"����
lib /def:libgnutls-28.def
�� libgnutls-28.lib���쐬�������N����K�v������܂�

���̂܂܂ł̓����N�G���[���o��̂�
gnutls.h�� 1470�s��
extern   gnutls_free_function gnutls_free;
��
__declspec(dllimport)  gnutls_free_function gnutls_free;
�ɕύX����K�v������܂�
