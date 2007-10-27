/** @file
 *
 * VBox frontends: Qt GUI ("VirtualBox"):
 * X11 keyboard driver translation tables (keyboard layouts)
 *
 */

/*
 * Copyright (C) 2007 innotek GmbH
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
USA
 */

#ifndef ___VBox_keyboard_tables_h
# error This file must be included from within keyboard-layouts.h
#endif /* ___VBox_keyboard_tables_h */

/* This file contains a list of the keyboard layouts in 
   keyboard-layouts.h, along with the name of the layout, for the
   library to run through when it is trying to guess the current
   layout. */
 
 {"U.S. English", &main_key_us},
 {"U.S. English, International (with dead keys)", &main_key_us_intl},
 {"U.S. English, Dvorak", &main_key_us_dvorak},
 {"U.S. English, Left handed Dvorak", &main_key_us_dvorak_l},
 {"U.S. English, Right handed Dvorak", &main_key_us_dvorak_r},
 {"U.S. English, Classic Dvorak", &main_key_us_dvorak_classic},
 {"U.S. English, Russian phonetic", &main_key_us_rus},
 {"Afghanistan", &main_key_af},
 {"Afghanistan, Pashto", &main_key_af_ps},
 {"Afghanistan, Southern Uzbek", &main_key_af_uz},
 {"Arabic", &main_key_ara},
 {"Arabic, azerty", &main_key_ara_azerty},
 {"Arabic, azerty/digits", &main_key_ara_azerty_digits},
 {"Arabic, digits", &main_key_ara_digits},
 {"Arabic, Buckwalter", &main_key_ara_buckwalter},
 {"Albania", &main_key_al},
 {"Armenia", &main_key_am},
 {"Armenia, Phonetic", &main_key_am_phonetic},
 {"Armenia, Eastern", &main_key_am_eastern},
 {"Armenia, Western", &main_key_am_western},
 {"Armenia, Alternative Eastern", &main_key_am_eastern_alt},
 {"Azerbaijan", &main_key_az},
 {"Azerbaijan, Cyrillic", &main_key_az_cyrillic},
 {"Belarus", &main_key_by},
 {"Belarus, Winkeys", &main_key_by_winkeys},
 {"Belgium", &main_key_be},
 {"Belgium, Eliminate dead keys", &main_key_be_nodeadkeys},
 {"Belgium, Sun dead keys", &main_key_be_sundeadkeys},
 {"Bangladesh", &main_key_bd},
 {"Bangladesh, Probhat", &main_key_bd_probhat},
 {"India", &main_key_in},
 {"India, Bengali", &main_key_in_ben},
 {"India, Gujarati", &main_key_in_guj},
 {"India, Gurmukhi", &main_key_in_guru},
 {"India, Kannada", &main_key_in_kan},
 {"India, Malayalam", &main_key_in_mal},
 {"India, Oriya", &main_key_in_ori},
 {"India, Tamil Unicode", &main_key_in_tam_unicode},
 {"India, Tamil TAB Typewriter", &main_key_in_tam_TAB},
 {"India, Tamil TSCII Typewriter", &main_key_in_tam_TSCII},
 {"India, Tamil", &main_key_in_tam},
 {"India, Telugu", &main_key_in_tel},
 {"India, Urdu", &main_key_in_urd},
 {"Bosnia and Herzegovina", &main_key_ba},
 {"Bosnia and Herzegovina, Use Bosnian digraphs", &main_key_ba_unicode},
 {"Bosnia and Herzegovina, US keyboard with Bosnian digraphs", &main_key_ba_unicodeus},
 {"Bosnia and Herzegovina, US keyboard with Bosnian letters", &main_key_ba_us},
 {"Brazil", &main_key_br},
 {"Brazil, Eliminate dead keys", &main_key_br_nodeadkeys},
 {"Bulgaria", &main_key_bg},
 {"Bulgaria, Phonetic", &main_key_bg_phonetic},
 {"Myanmar", &main_key_mm},
 {"Canada", &main_key_ca},
 {"Canada, French Dvorak", &main_key_ca_fr_dvorak},
 {"Canada, French (legacy)", &main_key_ca_fr_legacy},
 {"Canada, Multilingual", &main_key_ca_multix},
 {"Canada, Multilingual, second part", &main_key_ca_multi_2gr},
 {"Canada, Inuktitut", &main_key_ca_ike},
 {"Congo, Democratic Republic of the", &main_key_cd},
 {"Czechia", &main_key_cz},
 {"Czechia, With <|> key", &main_key_cz_bksl},
 {"Czechia, qwerty", &main_key_cz_qwerty},
 {"Czechia, qwerty, extended Backslash", &main_key_cz_qwerty_bksl},
 {"Denmark", &main_key_dk},
 {"Denmark, Eliminate dead keys", &main_key_dk_nodeadkeys},
 {"Netherlands", &main_key_nl},
 {"Bhutan", &main_key_bt},
 {"Estonia", &main_key_ee},
 {"Estonia, Eliminate dead keys", &main_key_ee_nodeadkeys},
 {"Iran", &main_key_ir},
 {"Iran, Kurdish, Latin Q", &main_key_ir_ku},
 {"Iran, Kurdish, (F)", &main_key_ir_ku_f},
 {"Iran, Kurdish, Arabic-Latin", &main_key_ir_ku_ara},
 {"Faroe Islands", &main_key_fo},
 {"Finland", &main_key_fi},
 {"Finland, Eliminate dead keys", &main_key_fi_nodeadkeys},
 {"Finland, Northern Saami", &main_key_fi_smi},
 {"Finland, Macintosh", &main_key_fi_mac},
 {"France", &main_key_fr},
 {"France, Eliminate dead keys", &main_key_fr_nodeadkeys},
 {"France, Sun dead keys", &main_key_fr_sundeadkeys},
 {"France, Alternative", &main_key_fr_oss},
 {"France, Alternative, eliminate dead keys", &main_key_fr_oss_nodeadkeys},
 {"France, Alternative, Sun dead keys", &main_key_fr_oss_sundeadkeys},
 {"France, (Legacy) Alternative", &main_key_fr_latin9},
 {"France, (Legacy) Alternative, eliminate dead keys", &main_key_fr_latin9_nodeadkeys},
 {"France, (Legacy) Alternative, Sun dead keys", &main_key_fr_latin9_sundeadkeys},
 {"France, Dvorak", &main_key_fr_dvorak},
 {"France, Macintosh", &main_key_fr_mac},
 {"Ghana", &main_key_gh},
 {"Ghana, Akan", &main_key_gh_akan},
 {"Ghana, Ewe", &main_key_gh_ewe},
 {"Ghana, Fula", &main_key_gh_fula},
 {"Ghana, Ga", &main_key_gh_ga},
 {"Georgia", &main_key_ge},
 {"Georgia, Russian", &main_key_ge_ru},
 {"Germany", &main_key_de},
 {"Germany, Dead acute", &main_key_de_deadacute},
 {"Germany, Dead grave acute", &main_key_de_deadgraveacute},
 {"Germany, Eliminate dead keys", &main_key_de_nodeadkeys},
 {"Germany, Dvorak", &main_key_de_dvorak},
 {"Germany, Neostyle", &main_key_de_neo},
 {"Greece", &main_key_gr},
 {"Greece, Eliminate dead keys", &main_key_gr_nodeadkeys},
 {"Greece, Polytonic", &main_key_gr_polytonic},
 {"Hungary", &main_key_hu},
 {"Hungary, qwerty", &main_key_hu_qwerty},
 {"Hungary, 101/qwertz/comma/Dead keys", &main_key_hu_101_qwertz_comma_dead},
 {"Hungary, 102/qwerty/comma/Dead keys", &main_key_hu_102_qwerty_comma_dead},
 {"Iceland", &main_key_is},
 {"Iceland, Sun dead keys", &main_key_is_Sundeadkeys},
 {"Iceland, Eliminate dead keys", &main_key_is_nodeadkeys},
 {"Iceland, Macintosh", &main_key_is_mac},
 {"Israel", &main_key_il},
 {"Israel, lyx", &main_key_il_lyx},
 {"Israel, Phonetic", &main_key_il_phonetic},
 {"Italy", &main_key_it},
 {"Italy, Macintosh", &main_key_it_mac},
 {"Japan", &main_key_jp},
 {"Kyrgyzstan", &main_key_kg},
 {"Cambodia", &main_key_kh},
 {"Kazakhstan", &main_key_kz},
 {"Kazakhstan, Russian with Kazakh", &main_key_kz_ruskaz},
 {"Kazakhstan, Kazakh with Russian", &main_key_kz_kazrus},
 {"Laos", &main_key_la},
 {"Latin American", &main_key_latam},
 {"Latin American, Eliminate dead keys", &main_key_latam_nodeadkeys},
 {"Latin American, Sun dead keys", &main_key_latam_sundeadkeys},
 {"Lithuania", &main_key_lt},
 {"Lithuania, Standard", &main_key_lt_std},
 {"Lithuania, US keyboard with Lithuanian letters", &main_key_lt_us},
 {"Lithuania, IBM (LST 1205-92)", &main_key_lt_ibm},
 {"Latvia, Apostrophe (') variant", &main_key_lv_apostrophe},
 {"Latvia, Tilde (~) variant", &main_key_lv_tilde},
 {"Latvia, F-letter (F) variant", &main_key_lv_fkey},
 {"Macedonia", &main_key_mk},
 {"Macedonia, Eliminate dead keys", &main_key_mk_nodeadkeys},
 {"Malta", &main_key_mt},
 {"Malta, Maltese keyboard with US layout", &main_key_mt_us},
 {"Mongolia", &main_key_mn},
 {"Norway", &main_key_no},
 {"Norway, Eliminate dead keys", &main_key_no_nodeadkeys},
 {"Norway, Dvorak", &main_key_no_dvorak},
 {"Norway, Northern Saami", &main_key_no_smi},
 {"Norway, Macintosh", &main_key_no_mac},
 {"Norway, Macintosh, eliminate dead keys", &main_key_no_mac_nodeadkeys},
 {"Poland, qwertz", &main_key_pl_qwertz},
 {"Portugal", &main_key_pt},
 {"Portugal, Eliminate dead keys", &main_key_pt_nodeadkeys},
 {"Portugal, Sun dead keys", &main_key_pt_sundeadkeys},
 {"Portugal, Macintosh", &main_key_pt_mac},
 {"Portugal, Macintosh, eliminate dead keys", &main_key_pt_mac_nodeadkeys},
 {"Portugal, Macintosh, Sun dead keys", &main_key_pt_mac_sundeadkeys},
 {"Romania", &main_key_ro},
 {"Romania, Standard", &main_key_ro_std},
 {"Romania, Standard (Commabelow)", &main_key_ro_academic},
 {"Romania, Winkeys", &main_key_ro_winkeys},
 {"Russia", &main_key_ru},
 {"Russia, Phonetic", &main_key_ru_phonetic},
 {"Russia, Typewriter", &main_key_ru_typewriter},
 {"Russia, Tatar", &main_key_ru_tt},
 {"Russia, Ossetian", &main_key_ru_os},
 {"Russia, Ossetian, Winkeys", &main_key_ru_os_winkeys},
 {"Serbia and Montenegro", &main_key_cs},
 {"Serbia and Montenegro, Z and ZHE swapped", &main_key_cs_yz},
 {"Slovakia", &main_key_sk},
 {"Slovakia, Extended Backslash", &main_key_sk_bksl},
 {"Slovakia, qwerty", &main_key_sk_qwerty},
 {"Slovakia, qwerty, extended Backslash", &main_key_sk_qwerty_bksl},
 {"Spain", &main_key_es},
 {"Spain, Eliminate dead keys", &main_key_es_nodeadkeys},
 {"Spain, Sun dead keys", &main_key_es_sundeadkeys},
 {"Spain, Dvorak", &main_key_es_dvorak},
 {"Sweden, Dvorak", &main_key_se_dvorak},
 {"Sweden, Russian phonetic", &main_key_se_rus},
 {"Switzerland", &main_key_ch},
 {"Switzerland, German, eliminate dead keys", &main_key_ch_de_nodeadkeys},
 {"Switzerland, German, Sun dead keys", &main_key_ch_de_sundeadkeys},
 {"Switzerland, French", &main_key_ch_fr},
 {"Switzerland, French, eliminate dead keys", &main_key_ch_fr_nodeadkeys},
 {"Switzerland, French, Sun dead keys", &main_key_ch_fr_sundeadkeys},
 {"Syria, Syriac", &main_key_sy_syc},
 {"Syria, Syriac phonetic", &main_key_sy_syc_phonetic},
 {"Tajikistan", &main_key_tj},
 {"Sri Lanka", &main_key_lk},
 {"Thailand", &main_key_th},
 {"Thailand, TIS-820.2538", &main_key_th_tis},
 {"Thailand, Pattachote", &main_key_th_pat},
 {"Turkey", &main_key_tr},
 {"Turkey, (F)", &main_key_tr_f},
 {"Ukraine", &main_key_ua},
 {"Ukraine, Phonetic", &main_key_ua_phonetic},
 {"Ukraine, Typewriter", &main_key_ua_typewriter},
 {"Ukraine, Winkeys", &main_key_ua_winkeys},
 {"Ukraine, Standard RSTU", &main_key_ua_rstu},
 {"Ukraine, Standard RSTU on Russian layout", &main_key_ua_rstu_ru},
 {"United Kingdom", &main_key_gb},
 {"United Kingdom, International (with dead keys)", &main_key_gb_intl},
 {"United Kingdom, Dvorak", &main_key_gb_dvorak},
 {"United Kingdom, Macintosh", &main_key_gb_mac},
 {"Uzbekistan", &main_key_uz},
 {"Vietnam", &main_key_vn},
 {"Japan (PC-98xx Series)", &main_key_nec_vndr_jp},
 {"Ireland, Ogham", &main_key_ie_ogam},
 {"Ireland, Ogham IS434", &main_key_ie_ogam_is434},
 {"Maldives", &main_key_mv},
 {"Esperanto", &main_key_epo},
 {"Nepal", &main_key_np},
 {"Nigeria", &main_key_ng},
 {"Nigeria, Igbo", &main_key_ng_igbo},
 {"Nigeria, Yoruba", &main_key_ng_yoruba},
 {"Nigeria, Hausa", &main_key_ng_hausa},
