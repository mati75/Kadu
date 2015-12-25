project (mime_tex)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.8.0")
set (CMAKE_MIN_VERSION "2.8.0")

find_package (Kadu REQUIRED CONFIG)

add_subdirectory (mimetex)
set (SOURCES
	formula_view.cpp
	formula_widget.cpp
	mime_tex.cpp
	mime_tex_plugin.cpp
	tex_formula_dialog.cpp
)

set (CONFIGURATION_FILES
	configuration/mime_tex.ui)

set (DATA_FILES
	data/mime_tex_32x32.png
)
set (DATA_FILES_EDITOR_ICONS
	data/editor_icons/redo.png
	data/editor_icons/undo.png
)
set (DATA_FILES_MIME_TEX_ICONS
	data/mime_tex_icons/arrows_tab_Downarrow.png
	data/mime_tex_icons/arrows_tab_icon.png
	data/mime_tex_icons/arrows_tab_leftarrow.png
	data/mime_tex_icons/arrows_tab_Leftarrow.png
	data/mime_tex_icons/arrows_tab_leftharpoondown.png
	data/mime_tex_icons/arrows_tab_leftharpoonup.png
	data/mime_tex_icons/arrows_tab_leftrightarrow.png
	data/mime_tex_icons/arrows_tab_Leftrightarrow.png
	data/mime_tex_icons/arrows_tab_longleftarrow.png
	data/mime_tex_icons/arrows_tab_Longleftarrow.png
	data/mime_tex_icons/arrows_tab_longleftrightarrow.png
	data/mime_tex_icons/arrows_tab_Longleftrightarrow.png
	data/mime_tex_icons/arrows_tab_longrightarrow.png
	data/mime_tex_icons/arrows_tab_Longrightarrow.png
	data/mime_tex_icons/arrows_tab_nearrow.png
	data/mime_tex_icons/arrows_tab_nwarrow.png
	data/mime_tex_icons/arrows_tab_rightarrow.png
	data/mime_tex_icons/arrows_tab_Rightarrow.png
	data/mime_tex_icons/arrows_tab_rightharpoondown.png
	data/mime_tex_icons/arrows_tab_rightharpoonup.png
	data/mime_tex_icons/arrows_tab_rightleftharpoons.png
	data/mime_tex_icons/arrows_tab_searrow.png
	data/mime_tex_icons/arrows_tab_swarrow.png
	data/mime_tex_icons/arrows_tab_uparrow.png
	data/mime_tex_icons/arrows_tab_Uparrow.png
	data/mime_tex_icons/arrows_tab_updownarrow.png
	data/mime_tex_icons/arrows_tab_Updownarrow.png
	data/mime_tex_icons/arrows_tab_vec.png
	data/mime_tex_icons/delimiters_tab_hat.png
	data/mime_tex_icons/delimiters_tab_icon.png
	data/mime_tex_icons/delimiters_tab_langle.png
	data/mime_tex_icons/delimiters_tab_lbrace.png
	data/mime_tex_icons/delimiters_tab_left_bracket.png
	data/mime_tex_icons/delimiters_tab_left_lbrace.png
	data/mime_tex_icons/delimiters_tab_left_square_bracket.png
	data/mime_tex_icons/delimiters_tab_overbrace_desc.png
	data/mime_tex_icons/delimiters_tab_overbrace.png
	data/mime_tex_icons/delimiters_tab_overline.png
	data/mime_tex_icons/delimiters_tab_rangle.png
	data/mime_tex_icons/delimiters_tab_rbrace.png
	data/mime_tex_icons/delimiters_tab_right_bracket.png
	data/mime_tex_icons/delimiters_tab_right_rbrace.png
	data/mime_tex_icons/delimiters_tab_right_square_bracket.png
	data/mime_tex_icons/delimiters_tab_underbrace_desc.png
	data/mime_tex_icons/delimiters_tab_underbrace.png
	data/mime_tex_icons/delimiters_tab_underline.png
	data/mime_tex_icons/greek_letters_tab_alpha.png
	data/mime_tex_icons/greek_letters_tab_beta.png
	data/mime_tex_icons/greek_letters_tab_chi.png
	data/mime_tex_icons/greek_letters_tab_delta.png
	data/mime_tex_icons/greek_letters_tab_Delta.png
	data/mime_tex_icons/greek_letters_tab_epsilon.png
	data/mime_tex_icons/greek_letters_tab_eta.png
	data/mime_tex_icons/greek_letters_tab_gamma.png
	data/mime_tex_icons/greek_letters_tab_Gamma.png
	data/mime_tex_icons/greek_letters_tab_icon.png
	data/mime_tex_icons/greek_letters_tab_iota.png
	data/mime_tex_icons/greek_letters_tab_kappa.png
	data/mime_tex_icons/greek_letters_tab_lambda.png
	data/mime_tex_icons/greek_letters_tab_Lambda.png
	data/mime_tex_icons/greek_letters_tab_mu.png
	data/mime_tex_icons/greek_letters_tab_nu.png
	data/mime_tex_icons/greek_letters_tab_omega.png
	data/mime_tex_icons/greek_letters_tab_Omega.png
	data/mime_tex_icons/greek_letters_tab_phi.png
	data/mime_tex_icons/greek_letters_tab_Phi.png
	data/mime_tex_icons/greek_letters_tab_pi.png
	data/mime_tex_icons/greek_letters_tab_Pi.png
	data/mime_tex_icons/greek_letters_tab_psi.png
	data/mime_tex_icons/greek_letters_tab_Psi.png
	data/mime_tex_icons/greek_letters_tab_rho.png
	data/mime_tex_icons/greek_letters_tab_sigma.png
	data/mime_tex_icons/greek_letters_tab_Sigma.png
	data/mime_tex_icons/greek_letters_tab_tau.png
	data/mime_tex_icons/greek_letters_tab_theta.png
	data/mime_tex_icons/greek_letters_tab_Theta.png
	data/mime_tex_icons/greek_letters_tab_upsilon.png
	data/mime_tex_icons/greek_letters_tab_Upsilon.png
	data/mime_tex_icons/greek_letters_tab_varepsilon.png
	data/mime_tex_icons/greek_letters_tab_varphi.png
	data/mime_tex_icons/greek_letters_tab_varpi.png
	data/mime_tex_icons/greek_letters_tab_varrho.png
	data/mime_tex_icons/greek_letters_tab_varsigma.png
	data/mime_tex_icons/greek_letters_tab_vartheta.png
	data/mime_tex_icons/greek_letters_tab_xi.png
	data/mime_tex_icons/greek_letters_tab_Xi.png
	data/mime_tex_icons/greek_letters_tab_zeta.png
	data/mime_tex_icons/relations_tab_approx.png
	data/mime_tex_icons/relations_tab_bigcap.png
	data/mime_tex_icons/relations_tab_bigcup.png
	data/mime_tex_icons/relations_tab_cap.png
	data/mime_tex_icons/relations_tab_cup.png
	data/mime_tex_icons/relations_tab_equiv.png
	data/mime_tex_icons/relations_tab_frac.png
	data/mime_tex_icons/relations_tab_geq.png
	data/mime_tex_icons/relations_tab_gg.png
	data/mime_tex_icons/relations_tab_icon.png
	data/mime_tex_icons/relations_tab_index_lower.png
	data/mime_tex_icons/relations_tab_index_upper.png
	data/mime_tex_icons/relations_tab_in.png
	data/mime_tex_icons/relations_tab_int_from.png
	data/mime_tex_icons/relations_tab_int_from_to.png
	data/mime_tex_icons/relations_tab_int.png
	data/mime_tex_icons/relations_tab_leq.png
	data/mime_tex_icons/relations_tab_lim.png
	data/mime_tex_icons/relations_tab_ll.png
	data/mime_tex_icons/relations_tab_neq.png
	data/mime_tex_icons/relations_tab_ni.png
	data/mime_tex_icons/relations_tab_oint_from.png
	data/mime_tex_icons/relations_tab_oint_from_to.png
	data/mime_tex_icons/relations_tab_oint.png
	data/mime_tex_icons/relations_tab_pm.png
	data/mime_tex_icons/relations_tab_prod_from.png
	data/mime_tex_icons/relations_tab_prod_from_to.png
	data/mime_tex_icons/relations_tab_prod.png
	data/mime_tex_icons/relations_tab_simeq.png
	data/mime_tex_icons/relations_tab_sim.png
	data/mime_tex_icons/relations_tab_sqrt_n.png
	data/mime_tex_icons/relations_tab_sqrt.png
	data/mime_tex_icons/relations_tab_subseteq.png
	data/mime_tex_icons/relations_tab_subset.png
	data/mime_tex_icons/relations_tab_sum_from.png
	data/mime_tex_icons/relations_tab_sum_from_to.png
	data/mime_tex_icons/relations_tab_sum.png
	data/mime_tex_icons/relations_tab_supseteq.png
	data/mime_tex_icons/relations_tab_supset.png
	data/mime_tex_icons/symbols_tab_aleph.png
	data/mime_tex_icons/symbols_tab_angle.png
	data/mime_tex_icons/symbols_tab_bot.png
	data/mime_tex_icons/symbols_tab_cdot.png
	data/mime_tex_icons/symbols_tab_cdots.png
	data/mime_tex_icons/symbols_tab_ddots.png
	data/mime_tex_icons/symbols_tab_emptyset.png
	data/mime_tex_icons/symbols_tab_forall.png
	data/mime_tex_icons/symbols_tab_hbar.png
	data/mime_tex_icons/symbols_tab_icon.png
	data/mime_tex_icons/symbols_tab_Im.png
	data/mime_tex_icons/symbols_tab_infty.png
	data/mime_tex_icons/symbols_tab_ldots.png
	data/mime_tex_icons/symbols_tab_nabla.png
	data/mime_tex_icons/symbols_tab_partial.png
	data/mime_tex_icons/symbols_tab_prime.png
	data/mime_tex_icons/symbols_tab_Re.png
	data/mime_tex_icons/symbols_tab_surd.png
	data/mime_tex_icons/tex_icon.png
)

kadu_plugin (mime_tex
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/mime_tex)
install (FILES ${DATA_FILES_EDITOR_ICONS}
	DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/mime_tex/editor_icons)
install (FILES ${DATA_FILES_MIME_TEX_ICONS}
	DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/mime_tex/mime_tex_icons)

