/* ===================================
�@�@�@�@�@�@�@�@�@�@�@�@�@Proxydomo�@�@�@�@�@�@�@�@�@�@�@�@
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
�r���h�ɂ� boost(1.54~)�� zlib �� WTL �� ICU ���K�v�Ȃ̂ł��ꂼ��p�ӂ��Ă��������B
zlib�̃\�[�X�̏ꏊ
$(SolutionDir)zlib\zlib-1.2.5
zlib�̃��C�u�����̏ꏊ
$(SolutionDir)zlib\zlib125dll\static32\zlibstat.lib
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
