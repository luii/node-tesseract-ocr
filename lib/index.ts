/*
 * Copyright 2025 Philipp Czarnetzki
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * All available languages for tesseract
 * @readonly
 * @enum {string}
 */
export const Language = {
  afr: "afr",
  amh: "amh",
  ara: "ara",
  asm: "asm",
  aze: "aze",
  aze_cyrl: "aze_cyrl",
  bel: "bel",
  ben: "ben",
  bod: "bod",
  bos: "bos",
  bre: "bre",
  bul: "bul",
  cat: "cat",
  ceb: "ceb",
  ces: "ces",
  chi_sim: "chi_sim",
  chi_tra: "chi_tra",
  chr: "chr",
  cos: "cos",
  cym: "cym",
  dan: "dan",
  deu: "deu",
  deu_latf: "deu_latf",
  div: "div",
  dzo: "dzo",
  ell: "ell",
  eng: "eng",
  enm: "enm",
  epo: "epo",
  equ: "equ",
  est: "est",
  eus: "eus",
  fao: "fao",
  fas: "fas",
  fil: "fil",
  fin: "fin",
  fra: "fra",
  frm: "frm",
  fry: "fry",
  gla: "gla",
  gle: "gle",
  glg: "glg",
  grc: "grc",
  guj: "guj",
  hat: "hat",
  heb: "heb",
  hin: "hin",
  hrv: "hrv",
  hun: "hun",
  hye: "hye",
  iku: "iku",
  ind: "ind",
  isl: "isl",
  ita: "ita",
  ita_old: "ita_old",
  jav: "jav",
  jpn: "jpn",
  kan: "kan",
  kat: "kat",
  kat_old: "kat_old",
  kaz: "kaz",
  khm: "khm",
  kir: "kir",
  kmr: "kmr",
  kor: "kor",
  kor_vert: "kor_vert",
  kur: "kur",
  lao: "lao",
  lat: "lat",
  lav: "lav",
  lit: "lit",
  ltz: "ltz",
  mal: "mal",
  mar: "mar",
  mkd: "mkd",
  mlt: "mlt",
  mon: "mon",
  mri: "mri",
  msa: "msa",
  mya: "mya",
  nep: "nep",
  nld: "nld",
  nor: "nor",
  oci: "oci",
  ori: "ori",
  osd: "osd",
  pan: "pan",
  pol: "pol",
  por: "por",
  pus: "pus",
  que: "que",
  ron: "ron",
  rus: "rus",
  san: "san",
  sin: "sin",
  slk: "slk",
  slv: "slv",
  snd: "snd",
  spa: "spa",
  spa_old: "spa_old",
  sqi: "sqi",
  srp: "srp",
  srp_latn: "srp_latn",
  sun: "sun",
  swa: "swa",
  swe: "swe",
  syr: "syr",
  tam: "tam",
  tat: "tat",
  tel: "tel",
  tgk: "tgk",
  tha: "tha",
  tir: "tir",
  ton: "ton",
  tur: "tur",
  uig: "uig",
  ukr: "ukr",
  urd: "urd",
  uzb: "uzb",
  uzb_cyrl: "uzb_cyrl",
  vie: "vie",
  yid: "yid",
  yor: "yor",
} as const;

export type Language = (typeof Language)[keyof typeof Language];

/**
 * When Tesseract/Cube is initialized we can choose to instantiate/load/run
 * only the Tesseract part, only the Cube part or both along with the combiner.
 * The preference of which engine to use is stored in tessedit_ocr_engine_mode.
 * @readonly
 * @enum {number}
 */
export const OcrEngineModes = {
  /**
   * Run Tesseract only - fastest
   * @deprecated
   * @type {number}
   */
  OEM_TESSERACT_ONLY: 0,

  /**
   * Run just the LSTM line recognizer.
   * @type {nmumber}
   */
  OEM_LSTM_ONLY: 1,

  /**
   * Run the LSTM recognizer, but allow fallback
   * to Tesseract when things get difficult.
   * @deprecated
   * @type {number}
   */
  OEM_TESSERACT_LSTM_COMBINED: 2,

  /**
   * Specify this mode when calling init(),
   * to indicate that any of the above modes
   * should be automatically inferred from the
   * variables in the language-specific config,
   * command-line configs, or if not specified
   * in any of the above should be set to the
   * default OEM_TESSERACT_ONLY.
   * @type {number}
   * @default
   */
  OEM_DEFAULT: 3,
} as const;

export type OcrEngineMode =
  (typeof OcrEngineModes)[keyof typeof OcrEngineModes];

/**
 * Possible modes for page layout analysis.
 * @readonly
 * @enum {number}
 */
export const PageSegmentationModes = {
  // Orientation and script detection only.
  PSM_OSD_ONLY: 0,
  // Automatic page segmentation with orientation and script detection. (OSD)
  PSM_AUTO_OSD: 1,
  // Automatic page segmentation, but no OSD, or OCR.
  PSM_AUTO_ONLY: 2,
  // Fully automatic page segmentation, but no OSD.
  PSM_AUTO: 3,
  // Assume a single column of text of variable sizes.
  PSM_SINGLE_COLUMN: 4,
  // Assume a single uniform block of vertically aligned text.
  PSM_SINGLE_BLOCK_VERT_TEXT: 5,
  // Assume a single uniform block of text. (Default.)
  PSM_SINGLE_BLOCK: 6,
  // Treat the image as a single text line.
  PSM_SINGLE_LINE: 7,
  // Treat the image as a single word.
  PSM_SINGLE_WORD: 8,
  // Treat the image as a single word in a circle.
  PSM_CIRCLE_WORD: 9,
  // Treat the image as a single character.
  PSM_SINGLE_CHAR: 10,
  // Find as much text as possible in no particular order.
  PSM_SPARSE_TEXT: 11,
  // Sparse text with orientation and script det.
  PSM_SPARSE_TEXT_OSD: 12,
  // Treat the image as a single text line, bypassing hacks that are Tesseract-specific.
  PSM_RAW_LINE: 13,
} as const;

export type PageSegmentationMode =
  (typeof PageSegmentationModes)[keyof typeof PageSegmentationModes];

export const LogLevels = {
  ALL: "-2147483648",
  TRACE: "5000",
  DEBUG: "10000",
  INFO: "20000",
  WARN: "30000",
  ERROR: "40000",
  FATAL: "50000",
  OFF: "2147483647",
} as const;

export type LogLevel = (typeof LogLevels)[keyof typeof LogLevels];

export type ConfigurationVariables = {
  log_level: `${LogLevel}`; // Logging level
  textord_dotmatrix_gap: `${number}`; // Max pixel gap for broken pixed pitch
  textord_debug_block: `${0 | 1}`; // Block to do debug on
  textord_pitch_range: `${number}`; // Max range test on pitch
  textord_words_veto_power: `${number}`; // Rows required to outvote a veto
  textord_tabfind_show_strokewidths: `${0 | 1}`; // Show stroke widths (ScrollView)
  pitsync_linear_version: `${number}`; // Use new fast algorithm
  oldbl_holed_losscount: `${number}`; // Max lost before fallback line used
  textord_skewsmooth_offset: `${number}`; // For smooth factor
  textord_skewsmooth_offset2: `${0 | 1}`; // For smooth factor
  textord_test_x: `${number}`; // coord of test pt
  textord_test_y: `${number}`; // coord of test pt
  textord_min_blobs_in_row: `${number}`; // Min blobs before gradient counted
  textord_spline_minblobs: `${number}`; // Min blobs in each spline segment
  textord_spline_medianwin: `${number}`; // Size of window for spline segmentation
  textord_max_blob_overlaps: `${number}`; // Max number of blobs a big blob can overlap
  textord_min_xheight: `${number}`; // Min credible pixel xheight
  textord_lms_line_trials: `${number}`; // Number of linew fits to do
  textord_tabfind_show_images: `${0 | 1}`; // Show image blobs
  textord_fp_chop_error: `${number}`; // Max allowed bending of chop cells
  edges_max_children_per_outline: `${number}`; // Max number of children inside a character outline
  edges_max_children_layers: `${number}`; // Max layers of nested children inside a character outline
  edges_children_per_grandchild: `${number}`; // Importance ratio for chucking outlines
  edges_children_count_limit: `${number}`; // Max holes allowed in blob
  edges_min_nonhole: `${number}`; // Min pixels for potential char in box
  edges_patharea_ratio: `${number}`; // Max lensq/area for acceptable child outline
  devanagari_split_debuglevel: `${0 | 1}`; // Debug level for split shiro-rekha process.
  textord_tabfind_show_partitions: `${0 | 1}`; // Show partition bounds, waiting if >1 (ScrollView)
  textord_debug_tabfind: `${0 | 1}`; // Debug tab finding
  textord_debug_bugs: `${0 | 1}`; // Turn on output related to bugs in tab finding
  textord_testregion_left: `${number}`; // Left edge of debug reporting rectangle in Leptonica coords (bottom=0/top=height), with horizontal lines x/y-flipped
  textord_testregion_top: `${number}`; // Top edge of debug reporting rectangle in Leptonica coords (bottom=0/top=height), with horizontal lines x/y-flipped
  textord_testregion_right: `${number}`; // Right edge of debug rectangle in Leptonica coords (bottom=0/top=height), with horizontal lines x/y-flipped
  textord_testregion_bottom: `${number}`; // Bottom edge of debug rectangle in Leptonica coords (bottom=0/top=height), with horizontal lines x/y-flipped
  classify_num_cp_levels: `${number}`; // Number of Class Pruner Levels
  editor_image_xpos: `${number}`; // Editor image X Pos
  editor_image_ypos: `${number}`; // Editor image Y Pos
  editor_image_menuheight: `${number}`; // Add to image height for menu bar                                                                                                       editor_image_word_bb_color: `${number}`; // Word bounding box colour
  editor_image_blob_bb_color: `${number}`; // Blob bounding box colour                                                                                                            editor_word_xpos: `${number}`; // Word window X Pos
  editor_word_ypos: `${number}`; // Word window Y Pos                                                                                                                             editor_word_height: `${number}`; // Word window height
  editor_word_width: `${number}`; // Word window width
  curl_timeout: `${0 | 1}`; // Timeout for curl in seconds
  wordrec_display_all_blobs: `${0 | 1}`; // Display Blobs
  wordrec_blob_pause: `${0 | 1}`; // Blob pause
  textord_force_make_prop_words: `${0 | 1}`; // Force proportional word segmentation on all rows
  textord_chopper_test: `${0 | 1}`; // Chopper is being tested.
  textord_restore_underlines: `${0 | 1}`; // Chop underlines & put back
  textord_show_initial_words: `${0 | 1}`; // Display separate words
  textord_blocksall_fixed: `${0 | 1}`; // Moan about prop blocks
  textord_blocksall_prop: `${0 | 1}`; // Moan about fixed pitch blocks
  textord_pitch_scalebigwords: `${0 | 1}`; // Scale scores on big words                                                                                                           textord_all_prop: `${0 | 1}`; // All doc is proportial text
  textord_debug_pitch_test: `${0 | 1}`; // Debug on fixed pitch test
  textord_disable_pitch_test: `${0 | 1}`; // Turn off dp fixed pitch algorithm
  textord_fast_pitch_test: `${0 | 1}`; // Do even faster pitch algorithm
  textord_debug_pitch_metric: `${0 | 1}`; // Write full metric stuff
  textord_show_row_cuts: `${0 | 1}`; // Draw row-level cuts
  textord_show_page_cuts: `${0 | 1}`; // Draw page-level cuts
  textord_blockndoc_fixed: `${0 | 1}`; // Attempt whole doc/block fixed pitch
  textord_show_tables: `${0 | 1}`; // Show table regions (ScrollView)
  textord_tablefind_show_mark: `${0 | 1}`; // Debug table marking steps in detail (ScrollView)
  textord_tablefind_show_stats: `${0 | 1}`; // Show page stats used in table finding (ScrollView)
  textord_tablefind_recognize_tables: `${0 | 1}`; // Enables the table recognizer for table layout and filtering.
  textord_tabfind_show_initialtabs: `${0 | 1}`; // Show tab candidates
  textord_tabfind_show_finaltabs: `${0 | 1}`; // Show tab vectors
  textord_tabfind_only_strokewidths: `${0 | 1}`; // Only run stroke widths
  textord_really_old_xheight: `${0 | 1}`; // Use original wiseowl xheight
  textord_oldbl_debug: `${0 | 1}`; // Debug old baseline generation
  textord_debug_baselines: `${0 | 1}`; // Debug baseline generation
  textord_oldbl_paradef: `${0 | 1}`; // Use para default mechanism
  textord_oldbl_split_splines: `${0 | 1}`; // Split stepped splines
  textord_oldbl_merge_parts: `${0 | 1}`; // Merge suspect partitions
  oldbl_corrfix: `${0 | 1}`; // Improve correlation of heights
  oldbl_xhfix: `${0 | 1}`; // Fix bug in modes threshold for xheights
  textord_ocropus_mode: `${0 | 1}`; // Make baselines for ocropus
  textord_heavy_nr: `${0 | 1}`; // Vigorously remove noise
  textord_show_initial_rows: `${0 | 1}`; // Display row accumulation
  textord_show_parallel_rows: `${0 | 1}`; // Display page correlated rows
  textord_show_expanded_rows: `${0 | 1}`; // Display rows after expanding
  textord_show_final_rows: `${0 | 1}`; // Display rows after final fitting
  textord_show_final_blobs: `${0 | 1}`; // Display blob bounds after pre-ass
  textord_test_landscape: `${0 | 1}`; // Tests refer to land/port
  textord_parallel_baselines: `${0 | 1}`; // Force parallel baselines
  textord_straight_baselines: `${0 | 1}`; // Force straight baselines
  textord_old_baselines: `${0 | 1}`; // Use old baseline algorithm
  textord_old_xheight: `${0 | 1}`; // Use old xheight algorithm
  textord_fix_xheight_bug: `${0 | 1}`; // Use spline baseline
  textord_fix_makerow_bug: `${0 | 1}`; // Prevent multiple baselines
  textord_debug_xheights: `${0 | 1}`; // Test xheight algorithms
  textord_biased_skewcalc: `${0 | 1}`; // Bias skew estimates with line length
  textord_interpolating_skew: `${0 | 1}`; // Interpolate across gaps
  textord_new_initial_xheight: `${0 | 1}`; // Use test xheight mechanism
  textord_debug_blob: `${0 | 1}`; // Print test blob information
  gapmap_debug: `${0 | 1}`; // Say which blocks have tables
  gapmap_use_ends: `${0 | 1}`; // Use large space at start and end of rows
  gapmap_no_isolated_quanta: `${0 | 1}`; // Ensure gaps not less than 2quanta wide
  edges_use_new_outline_complexity: `${0 | 1}`; // Use the new outline complexity module
  edges_debug: `${0 | 1}`; // turn on debugging for this module
  edges_children_fix: `${0 | 1}`; // Remove boxy parents of char-like children
  textord_show_fixed_cuts: `${0 | 1}`; // Draw fixed pitch cell boundaries
  devanagari_split_debugimage: `${0 | 1}`; // Whether to create a debug image for split shiro-rekha process.
  textord_tabfind_show_initial_partitions: `${0 | 1}`; // Show partition bounds
  textord_tabfind_show_reject_blobs: `${0 | 1}`; // Show blobs rejected as noise
  textord_tabfind_show_columns: `${0 | 1}`; // Show column bounds (ScrollView)
  textord_tabfind_show_blocks: `${0 | 1}`; // Show final block bounds (ScrollView)
  textord_tabfind_find_tables: `${0 | 1}`; // run table detection
  textord_space_size_is_variable: `${0 | 1}`; // If true, word delimiter spaces are assumed to have variable width, even though characters have fixed pitch.
  textord_debug_printable: `${0 | 1}`; // Make debug windows printable
  wordrec_display_splits: `${0 | 1}`; // Display splits
  poly_debug: `${0 | 1}`; // Debug old poly
  poly_wide_objects_better: `${0 | 1}`; // More accurate approx on wide things
  equationdetect_save_bi_image: `${0 | 1}`; // Save input bi image
  equationdetect_save_spt_image: `${0 | 1}`; // Save special character image
  equationdetect_save_seed_image: `${0 | 1}`; // Save the seed image
  equationdetect_save_merged_image: `${0 | 1}`; // Save the merged image                                                                                                          stream_filelist: `${0 | 1}`; // Stream a filelist from stdin
  debug_file: `${string}`; // to send tprintf output to                                                                                                                           editor_image_win_name: `${string}`; // Editor image window name
  editor_word_name: `${string}`; // BL normalized word window
  dotproduct: `${string}`; // Function used for calculation of dot product
  document_title: `${string}`; // of output document (used for hOCR and PDF output)
  curl_cookiefile: `${string}`; // with cookie data for curl
  classify_font_name: `${string}`; // Default font name to be used in training
  textord_underline_offset: `${number}`; // Fraction of x to ignore
  textord_wordstats_smooth_factor: `${number}`; // Smoothing gap stats
  textord_words_maxspace: `${number}`; // Multiple of xheight
  textord_words_default_maxspace: `${number}`; // Max believable third space
  textord_words_default_minspace: `${number}`; // Fraction of xheight
  textord_words_min_minspace: `${number}`; // Fraction of xheight
  textord_words_default_nonspace: `${number}`; // Fraction of xheight
  textord_words_initial_lower: `${number}`; // Max initial cluster size
  textord_words_initial_upper: `${number}`; // Min initial cluster spacing
  textord_words_minlarge: `${number}`; // Fraction of valid gaps needed
  textord_words_pitchsd_threshold: `${number}`; // Pitch sync threshold
  textord_words_def_fixed: `${number}`; // Threshold for definite fixed
  textord_words_def_prop: `${number}`; // Threshold for definite prop
  textord_pitch_rowsimilarity: `${number}`; // Fraction of xheight for sameness
  words_initial_lower: `${number}`; // Max initial cluster size
  words_initial_upper: `${number}`; // Min initial cluster spacing
  words_default_prop_nonspace: `${number}`; // Fraction of xheight
  words_default_fixed_space: `${number}`; // Fraction of xheight
  words_default_fixed_limit: `${number}`; // Allowed size variance
  textord_words_definite_spread: `${number}`; // Non-fuzzy spacing region
  textord_spacesize_ratioprop: `${number}`; // Min ratio space/nonspace
  textord_fpiqr_ratio: `${number}`; // Pitch IQR/Gap IQR threshold
  textord_max_pitch_iqr: `${number}`; // Xh fraction noise in pitch
  textord_projection_scale: `${number}`; // Ding rate for mid-cuts
  textord_balance_factor: `${0 | 1}`; // Ding rate for unbalanced char cells
  textord_tabvector_vertical_gap_fraction: `${number}`; // max fraction of mean blob width allowed for vertical gaps in vertical text
  textord_tabvector_vertical_box_ratio: `${number}`; // Fraction of box matches required to declare a line vertical
  pitsync_joined_edge: `${number}`; // Dist inside big blob for chopping
  pitsync_offset_freecut_fraction: `${number}`; // Fraction of cut for free cuts
  oldbl_xhfract: `${number}`; // Fraction of est allowed in calc
  oldbl_dot_error_size: `${number}`; // Max aspect ratio of a dot
  textord_oldbl_jumplimit: `${number}`; // X fraction for new partition
  textord_spline_shift_fraction: `${number}`; // Fraction of line spacing for quad
  textord_skew_ile: `${number}`; // Ile of gradients for page skew
  textord_skew_lag: `${number}`; // Lag for skew on row accumulation
  textord_linespace_iqrlimit: `${number}`; // Max iqr/median for linespace
  textord_width_limit: `${number}`; // Max width of blobs to make rows
  textord_chop_width: `${number}`; // Max width before chopping
  textord_expansion_factor: `${0 | 1}`; // Factor to expand rows by in expand_rows
  textord_overlap_x: `${number}`; // Fraction of linespace for good overlap
  textord_minxh: `${number}`; // fraction of linesize for min xheight
  textord_min_linesize: `${number}`; // * blob height for initial linesize
  textord_excess_blobsize: `${number}`; // New row made if blob makes row this big
  textord_occupancy_threshold: `${number}`; // Fraction of neighbourhood
  textord_underline_width: `${number}`; // Multiple of line_size for underline
  textord_min_blob_height_fraction: `${number}`; // Min blob height/top to include blob top into xheight stats
  textord_xheight_mode_fraction: `${number}`; // Min pile height to make xheight
  textord_ascheight_mode_fraction: `${number}`; // Min pile height to make ascheight
  textord_descheight_mode_fraction: `${number}`; // Min pile height to make descheight
  textord_ascx_ratio_min: `${number}`; // Min cap/xheight
  textord_ascx_ratio_max: `${number}`; // Max cap/xheight
  textord_descx_ratio_min: `${number}`; // Min desc/xheight
  textord_descx_ratio_max: `${number}`; // Max desc/xheight
  textord_xheight_error_margin: `${number}`; // Accepted variation
  gapmap_big_gaps: `${number}`; // xht multiplier
  edges_childarea: `${number}`; // Min area fraction of child outline
  edges_boxarea: `${number}`; // Min area fraction of grandchild for box
  textord_underline_threshold: `${number}`; // Fraction of width occupied
  classify_pico_feature_length: `${number}`; // Pico Feature Length
  classify_norm_adj_midpoint: `${number}`; // Norm adjust midpoint ...
  classify_norm_adj_curl: `${number}`; // Norm adjust curl ...
  classify_min_slope: `${number}`; // Slope below which lines are called horizontal
  classify_max_slope: `${number}`; // Slope above which lines are called vertical
  classify_cp_angle_pad_loose: `${number}`; // Class Pruner Angle Pad Loose
  classify_cp_angle_pad_medium: `${number}`; // Class Pruner Angle Pad Medium
  classify_cp_angle_pad_tight: `${number}`; // CLass Pruner Angle Pad Tight
  classify_cp_end_pad_loose: `${number}`; // Class Pruner End Pad Loose
  classify_cp_end_pad_medium: `${number}`; // Class Pruner End Pad Medium
  classify_cp_end_pad_tight: `${number}`; // Class Pruner End Pad Tight
  classify_cp_side_pad_loose: `${number}`; // Class Pruner Side Pad Loose
  classify_cp_side_pad_medium: `${number}`; // Class Pruner Side Pad Medium
  classify_cp_side_pad_tight: `${number}`; // Class Pruner Side Pad Tight
  classify_pp_angle_pad: `${number}`; // Proto Pruner Angle Pad
  classify_pp_end_pad: `${number}`; // Proto Prune End Pad
  classify_pp_side_pad: `${number}`; // Proto Pruner Side Pad
  ambigs_debug_level: `${0 | 1}`; // Debug level for unichar ambiguities
  classify_debug_level: `${0 | 1}`; // Classify debug level
  classify_norm_method: `${0 | 1}`; // Normalization Method ...
  matcher_debug_level: `${0 | 1}`; // Matcher Debug Level
  matcher_debug_flags: `${0 | 1}`; // Matcher Debug Flags
  classify_learning_debug_level: `${0 | 1}`; // Learning Debug Level:
  matcher_permanent_classes_min: `${0 | 1}`; // Min # of permanent classes
  matcher_min_examples_for_prototyping: `${number}`; // Reliable Config Threshold
  matcher_sufficient_examples_for_prototyping: `${number}`; // Enable adaption even if the ambiguities have not been seen
  classify_adapt_proto_threshold: `${number}`; // Threshold for good protos during adaptive 0-255
  classify_adapt_feature_threshold: `${number}`; // Threshold for good features during adaptive 0-255
  classify_class_pruner_threshold: `${number}`; // Class Pruner Threshold 0-255
  classify_class_pruner_multiplier: `${number}`; // Class Pruner Multiplier 0-255:
  classify_cp_cutoff_strength: `${number}`; // Class Pruner CutoffStrength:
  classify_integer_matcher_multiplier: `${number}`; // Integer Matcher Multiplier 0-255:
  dawg_debug_level: `${0 | 1}`; // Set to 1 for general debug info, to 2 for more details, to 3 to see all the debug messages
  hyphen_debug_level: `${0 | 1}`; // Debug level for hyphenated words.
  stopper_smallword_size: `${number}`; // Size of dict word to be treated as non-dict word
  stopper_debug_level: `${0 | 1}`; // Stopper debug level
  tessedit_truncate_wordchoice_log: `${number}`; // Max words to keep in list
  max_permuter_attempts: `${number}`; // Maximum number of different character choices to consider during permutation. This limit is especially useful when user patterns are specified, since overly generic patterns can result in dawg search exploring an overly large number of options.
  repair_unchopped_blobs: `${0 | 1}`; // Fix blobs that aren't chopped
  chop_debug: `${0 | 1}`; // Chop debug
  chop_split_length: `${number}`; // Split Length
  chop_same_distance: `${number}`; // Same distance
  chop_min_outline_points: `${number}`; // Min Number of Points on Outline
  chop_seam_pile_size: `${number}`; // Max number of seams in seam_pile
  chop_inside_angle: `${number}`; // Min Inside Angle Bend
  chop_min_outline_area: `${number}`; // Min Outline Area
  chop_centered_maxwidth: `${number}`; // Width of (smaller) chopped blobs above which we don't care that a chop is not near the center.
  chop_x_y_weight: `${number}`; // X / Y length weight
  wordrec_debug_level: `${0 | 1}`; // Debug level for wordrec
  wordrec_max_join_chunks: `${number}`; // Max number of broken pieces to associate
  segsearch_debug_level: `${0 | 1}`; // SegSearch debug level
  segsearch_max_pain_points: `${number}`; // Maximum number of pain points stored in the queue
  segsearch_max_futile_classifications: `${number}`; // Maximum number of pain point classifications per chunk that did not result in finding a better word choice.
  language_model_debug_level: `${0 | 1}`; // Language model debug level
  language_model_ngram_order: `${number}`; // Maximum order of the character ngram model
  language_model_viterbi_list_max_num_prunable: `${number}`; // Maximum number of prunable (those for which PrunablePath() is true) entries in each viterbi list recorded in BLOB_CHOICEs
  language_model_viterbi_list_max_size: `${number}`; // Maximum size of viterbi lists recorded in BLOB_CHOICEs
  language_model_min_compound_length: `${number}`; // Minimum length of compound words
  wordrec_display_segmentations: `${0 | 1}`; // Display Segmentations (ScrollView)
  tessedit_pageseg_mode: `${number}`; // Page seg mode: 0=osd only, 1=auto+osd, 2=auto_only, 3=auto, 4=column, 5=block_vert, 6=block, 7=line, 8=word, 9=word_circle, 10=char,11=sparse_text, 12=sparse_text+osd, 13=raw_line (Values from PageSegMode enum in tesseract/publictypes.h)
  thresholding_method: `${0 | 1}`; // Thresholding method: 0 = Otsu, 1 = LeptonicaOtsu, 2 = Sauvola
  tessedit_ocr_engine_mode: `${number}`; // Which OCR engine(s) to run (Tesseract, LSTM, both). Defaults to loading and running the most accurate available.
  pageseg_devanagari_split_strategy: `${0 | 1}`; // Whether to use the top-line splitting process for Devanagari documents while performing page-segmentation.
  ocr_devanagari_split_strategy: `${0 | 1}`; // Whether to use the top-line splitting process for Devanagari documents while performing ocr.
  bidi_debug: `${0 | 1}`; // Debug level for BiDi
  applybox_debug: `${0 | 1}`; // Debug level
  applybox_page: `${0 | 1}`; // Page number to apply boxes from
  tessedit_font_id: `${0 | 1}`; // Font ID to use or zero
  tessedit_bigram_debug: `${0 | 1}`; // Amount of debug output for bigram correction.
  debug_noise_removal: `${0 | 1}`; // Debug reassignment of small outlines
  noise_maxperblob: `${number}`; // Max diacritics to apply to a blob
  noise_maxperword: `${number}`; // Max diacritics to apply to a word
  debug_x_ht_level: `${0 | 1}`; // Reestimate debug
  quality_min_initial_alphas_reqd: `${number}`; // alphas in a good word
  tessedit_tess_adaption_mode: `${number}`; // Adaptation decision algorithm for tess
  multilang_debug_level: `${0 | 1}`; // Print multilang debug info.
  paragraph_debug_level: `${0 | 1}`; // Print paragraph debug info.
  tessedit_preserve_min_wd_len: `${number}`; // Only preserve wds longer than this
  crunch_rating_max: `${number}`; // For adj length in rating per ch
  crunch_pot_indicators: `${0 | 1}`; // How many potential indicators needed
  crunch_leave_lc_strings: `${number}`; // Don't crunch words with long lower case strings
  crunch_leave_uc_strings: `${number}`; // Don't crunch words with long lower case strings
  crunch_long_repetitions: `${number}`; // Crunch words with long repetitions
  crunch_debug: `${0 | 1}`; // As it says
  fixsp_non_noise_limit: `${0 | 1}`; // How many non-noise blbs either side?
  fixsp_done_mode: `${0 | 1}`; // What constitutes done for spacing
  debug_fix_space_level: `${0 | 1}`; // Contextual fixspace debug
  x_ht_acceptance_tolerance: `${number}`; // Max allowed deviation of blob top outside of font data
  x_ht_min_change: `${number}`; // Min change in xht before actually trying it
  superscript_debug: `${0 | 1}`; // Debug level for sub & superscript fixer
  page_xml_level: `${0 | 1}`; // Create the PAGE file on 0=line or 1=word level.
  jpg_quality: `${number}`; // Set JPEG quality level
  user_defined_dpi: `${0 | 1}`; // Specify DPI for input image
  min_characters_to_try: `${number}`; // Specify minimum characters to try during OSD
  suspect_level: `${number}`; // Suspect marker level
  suspect_short_words: `${number}`; // Don't suspect dict wds longer than this
  tessedit_reject_mode: `${0 | 1}`; // Rejection algorithm
  tessedit_image_border: `${number}`; // Rej blbs near image edge limit
  min_sane_x_ht_pixels: `${number}`; // Reject any x-ht lt or eq than this
  tessedit_page_number: `${number}`; // -1 -> All pages, else specific page to process
  tessedit_parallelize: `${0 | 1}`; // Run in parallel where possible
  lstm_choice_mode: `${0 | 1}`; // Allows to include alternative symbols choices in the hOCR output. Valid input values are 0, 1 and 2. 0 is the default value. With 1 the alternative symbol choices per timestep are included. With 2 alternative symbol choices are extracted from the CTC process instead of the lattice. The choices are mapped per character.
  lstm_choice_iterations: `${number}`; // Sets the number of cascading iterations for the Beamsearch in lstm_choice_mode. Note that lstm_choice_mode must be set to a value greater than 0 to produce results.
  tosp_debug_level: `${0 | 1}`; // Debug data
  tosp_enough_space_samples_for_median: `${number}`; // or should we use mean
  tosp_redo_kern_limit: `${number}`; // No.samples reqd to reestimate for row
  tosp_few_samples: `${number}`; // No.gaps reqd with 1 large gap to treat as a table
  tosp_short_row: `${number}`; // No.gaps reqd with few cert spaces to use certs
  tosp_sanity_method: `${0 | 1}`; // How to avoid being silly
  textord_max_noise_size: `${number}`; // Pixel size of noise
  textord_baseline_debug: `${0 | 1}`; // Baseline debug level
  textord_noise_sizefraction: `${number}`; // Fraction of size for maxima
  textord_noise_translimit: `${number}`; // Transitions for normal blob
  textord_noise_sncount: `${0 | 1}`; // super norm blobs to save row
  use_ambigs_for_adaption: `${0 | 1}`; // Use ambigs for deciding whether to adapt to a character
  allow_blob_division: `${0 | 1}`; // Use divisible blobs chopping
  prioritize_division: `${0 | 1}`; // Prioritize blob division over chopping
  classify_enable_learning: `${0 | 1}`; // Enable adaptive classifier
  tess_cn_matching: `${0 | 1}`; // Character Normalized Matching
  tess_bn_matching: `${0 | 1}`; // Baseline Normalized Matching
  classify_enable_adaptive_matcher: `${0 | 1}`; // Enable adaptive classifier
  classify_use_pre_adapted_templates: `${0 | 1}`; // Use pre-adapted classifier templates
  classify_save_adapted_templates: `${0 | 1}`; // Save adapted templates to a file
  classify_enable_adaptive_debugger: `${0 | 1}`; // Enable match debugger
  classify_nonlinear_norm: `${0 | 1}`; // Non-linear stroke-density normalization
  disable_character_fragments: `${0 | 1}`; // Do not include character fragments in the results of the classifier
  classify_debug_character_fragments: `${0 | 1}`; // Bring up graphical debugging windows for fragments training
  matcher_debug_separate_windows: `${0 | 1}`; // Use two different windows for debugging the matching: One for the protos and one for the features.
  classify_bln_numeric_mode: `${0 | 1}`; // Assume the input is numbers [0-9].
  load_system_dawg: `${0 | 1}`; // Load system word dawg.
  load_freq_dawg: `${0 | 1}`; // Load frequent word dawg.
  load_unambig_dawg: `${0 | 1}`; // Load unambiguous word dawg.
  load_punc_dawg: `${0 | 1}`; // Load dawg with punctuation patterns.
  load_number_dawg: `${0 | 1}`; // Load dawg with number patterns.
  load_bigram_dawg: `${0 | 1}`; // Load dawg with special word bigrams.
  use_only_first_uft8_step: `${0 | 1}`; // Use only the first UTF8 step of the given string when computing log probabilities.
  stopper_no_acceptable_choices: `${0 | 1}`; // Make AcceptableChoice() always return false. Useful when there is a need to explore all segmentations
  segment_nonalphabetic_script: `${0 | 1}`; // Don't use any alphabetic-specific tricks. Set to true in the traineddata config file for scripts that are cursive or inherently fixed-pitch
  save_doc_words: `${0 | 1}`; // Save Document Words
  merge_fragments_in_matrix: `${0 | 1}`; // Merge the fragments in the ratings matrix and delete them after merging
  wordrec_enable_assoc: `${0 | 1}`; // Associator Enable
  force_word_assoc: `${0 | 1}`; // force associator to run regardless of what enable_assoc is. This is used for CJK where component grouping is necessary.
  chop_enable: `${0 | 1}`; // Chop enable
  chop_vertical_creep: `${0 | 1}`; // Vertical creep
  chop_new_seam_pile: `${0 | 1}`; // Use new seam_pile
  assume_fixed_pitch_char_segment: `${0 | 1}`; // include fixed-pitch heuristics in char segmentation
  wordrec_skip_no_truth_words: `${0 | 1}`; // Only run OCR for words that had truth recorded in BlamerBundle
  wordrec_debug_blamer: `${0 | 1}`; // Print blamer debug messages
  wordrec_run_blamer: `${0 | 1}`; // Try to set the blame for errors
  save_alt_choices: `${0 | 1}`; // Save alternative paths found during chopping and segmentation search
  language_model_ngram_on: `${0 | 1}`; // Turn on/off the use of character ngram model
  language_model_ngram_use_only_first_uft8_step: `${0 | 1}`; // Use only the first UTF8 step of the given string when computing log probabilities.
  language_model_ngram_space_delimited_language: `${0 | 1}`; // Words are delimited by space
  language_model_use_sigmoidal_certainty: `${0 | 1}`; // Use sigmoidal score for certainty
  tessedit_resegment_from_boxes: `${0 | 1}`; // Take segmentation and labeling from box file
  tessedit_resegment_from_line_boxes: `${0 | 1}`; // Conversion of word/line box file to char box file
  tessedit_train_from_boxes: `${0 | 1}`; // Generate training data from boxed chars
  tessedit_make_boxes_from_boxes: `${0 | 1}`; // Generate more boxes from boxed chars
  tessedit_train_line_recognizer: `${0 | 1}`; // Break input into lines and remap boxes if present
  tessedit_dump_pageseg_images: `${0 | 1}`; // Dump intermediate images made during page segmentation
  tessedit_do_invert: `${0 | 1}`; // Try inverted line image if necessary (deprecated, will be removed in release 6, use the 'invert_threshold' parameter instead)
  thresholding_debug: `${0 | 1}`; // Debug the thresholding process
  tessedit_ambigs_training: `${0 | 1}`; // Perform training for ambiguities
  tessedit_adaption_debug: `${0 | 1}`; // Generate and print debug information for adaption
  applybox_learn_chars_and_char_frags_mode: `${0 | 1}`; // Learn both character fragments (as is done in the special low exposure mode) as well as unfragmented characters.
  applybox_learn_ngrams_mode: `${0 | 1}`; // Each bounding box is assumed to contain ngrams. Only learn the ngrams whose outlines overlap horizontally.
  tessedit_display_outwords: `${0 | 1}`; // Draw output words
  tessedit_dump_choices: `${0 | 1}`; // Dump char choices
  tessedit_timing_debug: `${0 | 1}`; // Print timing stats
  tessedit_fix_fuzzy_spaces: `${0 | 1}`; // Try to improve fuzzy spaces
  tessedit_unrej_any_wd: `${0 | 1}`; // Don't bother with word plausibility
  tessedit_fix_hyphens: `${0 | 1}`; // Crunch double hyphens?
  tessedit_enable_doc_dict: `${0 | 1}`; // Add words to the document dictionary
  tessedit_debug_fonts: `${0 | 1}`; // Output font info per char
  tessedit_debug_block_rejection: `${0 | 1}`; // Block and Row stats
  tessedit_enable_bigram_correction: `${0 | 1}`; // Enable correction based on the word bigram dictionary.
  tessedit_enable_dict_correction: `${0 | 1}`; // Enable single word correction based on the dictionary.
  enable_noise_removal: `${0 | 1}`; // Remove and conditionally reassign small outlines when they confuse layout analysis, determining diacritics vs noise
  tessedit_minimal_rej_pass1: `${0 | 1}`; // Do minimal rejection on pass 1 output
  tessedit_test_adaption: `${0 | 1}`; // Test adaption criteria
  test_pt: `${0 | 1}`; // Test for point
  paragraph_text_based: `${0 | 1}`; // Run paragraph detection on the post-text-recognition (more accurate)
  lstm_use_matrix: `${0 | 1}`; // Use ratings matrix/beam search with lstm
  tessedit_good_quality_unrej: `${0 | 1}`; // Reduce rejection on good docs
  tessedit_use_reject_spaces: `${0 | 1}`; // Reject spaces?
  tessedit_preserve_blk_rej_perfect_wds: `${0 | 1}`; // Only rej partially rejected words in block rejection
  tessedit_preserve_row_rej_perfect_wds: `${0 | 1}`; // Only rej partially rejected words in row rejection
  tessedit_dont_blkrej_good_wds: `${0 | 1}`; // Use word segmentation quality metric
  tessedit_dont_rowrej_good_wds: `${0 | 1}`; // Use word segmentation quality metric
  tessedit_row_rej_good_docs: `${0 | 1}`; // Apply row rejection to good docs
  tessedit_reject_bad_qual_wds: `${0 | 1}`; // Reject all bad quality wds
  tessedit_debug_doc_rejection: `${0 | 1}`; // Page stats
  tessedit_debug_quality_metrics: `${0 | 1}`; // Output data to debug file
  bland_unrej: `${0 | 1}`; // unrej potential with no checks
  unlv_tilde_crunching: `${0 | 1}`; // Mark v.bad words for tilde crunch
  hocr_font_info: `${0 | 1}`; // Add font info to hocr output
  hocr_char_boxes: `${0 | 1}`; // Add coordinates for each character to hocr output
  crunch_early_merge_tess_fails: `${0 | 1}`; // Before word crunch?
  crunch_early_convert_bad_unlv_chs: `${0 | 1}`; // Take out ~^ early?
  crunch_terrible_garbage: `${0 | 1}`; // As it says
  crunch_leave_ok_strings: `${0 | 1}`; // Don't touch sensible strings
  crunch_accept_ok: `${0 | 1}`; // Use acceptability in okstring
  crunch_leave_accept_strings: `${0 | 1}`; // Don't pot crunch sensible strings
  crunch_include_numerals: `${0 | 1}`; // Fiddle alpha figures
  tessedit_prefer_joined_punct: `${0 | 1}`; // Reward punctuation joins
  tessedit_write_block_separators: `${0 | 1}`; // Write block separators in output
  tessedit_write_rep_codes: `${0 | 1}`; // Write repetition char code
  tessedit_write_unlv: `${0 | 1}`; // Write .unlv output file
  tessedit_create_txt: `${0 | 1}`; // Write .txt output file
  tessedit_create_hocr: `${0 | 1}`; // Write .html hOCR output file
  tessedit_create_alto: `${0 | 1}`; // Write .xml ALTO file
  tessedit_create_page_xml: `${0 | 1}`; // Write .page.xml PAGE file
  page_xml_polygon: `${0 | 1}`; // Create the PAGE file with polygons instead of box values
  tessedit_create_lstmbox: `${0 | 1}`; // Write .box file for LSTM training
  tessedit_create_tsv: `${0 | 1}`; // Write .tsv output file
  tessedit_create_wordstrbox: `${0 | 1}`; // Write WordStr format .box output file
  tessedit_create_pdf: `${0 | 1}`; // Write .pdf output file
  textonly_pdf: `${0 | 1}`; // Create PDF with only one invisible text layer
  suspect_constrain_1Il: `${0 | 1}`; // UNLV keep 1Il chars rejected
  tessedit_minimal_rejection: `${0 | 1}`; // Only reject tess failures
  tessedit_zero_rejection: `${0 | 1}`; // Don't reject ANYTHING
  tessedit_word_for_word: `${0 | 1}`; // Make output have exactly one word per WERD
  tessedit_zero_kelvin_rejection: `${0 | 1}`; // Don't reject ANYTHING AT ALL
  tessedit_rejection_debug: `${0 | 1}`; // Adaption debug
  tessedit_flip_0O: `${0 | 1}`; // Contextual 0O O0 flips
  rej_trust_doc_dawg: `${0 | 1}`; // Use DOC dawg in 11l conf. detector
  rej_1Il_use_dict_word: `${0 | 1}`; // Use dictword test
  rej_1Il_trust_permuter_type: `${0 | 1}`; // Don't double check
  rej_use_tess_accepted: `${0 | 1}`; // Individual rejection control
  rej_use_tess_blanks: `${0 | 1}`; // Individual rejection control
  rej_use_good_perm: `${0 | 1}`; // Individual rejection control
  rej_use_sensible_wd: `${0 | 1}`; // Extend permuter check
  rej_alphas_in_number_perm: `${0 | 1}`; // Extend permuter check
  tessedit_create_boxfile: `${0 | 1}`; // Output text with boxes
  tessedit_write_images: `${0 | 1}`; // Capture the image from the IPE
  interactive_display_mode: `${0 | 1}`; // Run interactively?
  tessedit_override_permuter: `${0 | 1}`; // According to dict_word
  tessedit_use_primary_params_model: `${0 | 1}`; // In multilingual mode use params model of the primary language
  textord_tabfind_show_vlines: `${0 | 1}`; // Debug line finding
  textord_use_cjk_fp_model: `${0 | 1}`; // Use CJK fixed pitch model
  poly_allow_detailed_fx: `${0 | 1}`; // Allow feature extractors to see the original outline
  tessedit_init_config_only: `${0 | 1}`; // Only initialize with the config file. Useful if the instance is not going to be used for OCR but say only for layout analysis.
  textord_equation_detect: `${0 | 1}`; // Turn on equation detector
  textord_tabfind_vertical_text: `${0 | 1}`; // Enable vertical detection
  textord_tabfind_force_vertical_text: `${0 | 1}`; // Force using vertical text page mode
  preserve_interword_spaces: `${0 | 1}`; // Preserve multiple interword spaces
  pageseg_apply_music_mask: `${0 | 1}`; // Detect music staff and remove intersecting components
  textord_single_height_mode: `${0 | 1}`; // Script has no xheight, so use a single mode
  tosp_old_to_method: `${0 | 1}`; // Space stats use prechopping?
  tosp_old_to_constrain_sp_kn: `${0 | 1}`; // Constrain relative values of inter and intra-word gaps for old_to_method.
  tosp_only_use_prop_rows: `${0 | 1}`; // Block stats to use fixed pitch rows?
  tosp_force_wordbreak_on_punct: `${0 | 1}`; // Force word breaks on punct to break long lines in non-space delimited langs
  tosp_use_pre_chopping: `${0 | 1}`; // Space stats use prechopping?
  tosp_old_to_bug_fix: `${0 | 1}`; // Fix suspected bug in old code
  tosp_block_use_cert_spaces: `${0 | 1}`; // Only stat OBVIOUS spaces
  tosp_row_use_cert_spaces: `${0 | 1}`; // Only stat OBVIOUS spaces
  tosp_narrow_blobs_not_cert: `${0 | 1}`; // Only stat OBVIOUS spaces
  tosp_row_use_cert_spaces1: `${0 | 1}`; // Only stat OBVIOUS spaces
  tosp_recovery_isolated_row_stats: `${0 | 1}`; // Use row alone when inadequate cert spaces
  tosp_only_small_gaps_for_kern: `${0 | 1}`; // Better guess
  tosp_all_flips_fuzzy: `${0 | 1}`; // Pass ANY flip to context?
  tosp_fuzzy_limit_all: `${0 | 1}`; // Don't restrict kn->sp fuzzy limit to tables
  tosp_stats_use_xht_gaps: `${0 | 1}`; // Use within xht gap for wd breaks
  tosp_use_xht_gaps: `${0 | 1}`; // Use within xht gap for wd breaks
  tosp_only_use_xht_gaps: `${0 | 1}`; // Only use within xht gap for wd breaks
  tosp_rule_9_test_punct: `${0 | 1}`; // Don't chng kn to space next to punct
  tosp_flip_fuzz_kn_to_sp: `${0 | 1}`; // Default flip
  tosp_flip_fuzz_sp_to_kn: `${0 | 1}`; // Default flip
  tosp_improve_thresh: `${0 | 1}`; // Enable improvement heuristic
  textord_no_rejects: `${0 | 1}`; // Don't remove noise blobs
  textord_show_blobs: `${0 | 1}`; // Display unsorted blobs
  textord_show_boxes: `${0 | 1}`; // Display unsorted blobs
  textord_noise_rejwords: `${0 | 1}`; // Reject noise-like words
  textord_noise_rejrows: `${0 | 1}`; // Reject noise-like rows
  textord_noise_debug: `${0 | 1}`; // Debug row garbage detector
  classify_learn_debug_str: `${string}`; // str to debug learning
  user_words_file: `${string}`; // filename of user-provided words.
  user_words_suffix: `${string}`; // suffix of user-provided words located in tessdata.
  user_patterns_file: `${string}`; // filename of user-provided patterns.
  user_patterns_suffix: `${string}`; // suffix of user-provided patterns located in tessdata.
  output_ambig_words_file: `${string}`; // file for ambiguities found in the dictionary
  word_to_debug: `${string}`; // for which stopper debug information should be printed to stdout
  tessedit_char_blacklist: `${string}`; // of chars not to recognize
  tessedit_char_whitelist: `${string}`; // of chars to recognize
  tessedit_char_unblacklist: `${string}`; // of chars to override tessedit_char_blacklist
  tessedit_write_params_to_file: `${string}`; // all parameters to the given file.
  applybox_exposure_pattern: `${string}`; // Exposure value follows this pattern in the image filename. The name of the image files are expected to be in the form [lang].[fontname].exp[num].tif
  chs_leading_punct: `${string}`; // Leading punctuation
  chs_trailing_punct1: `${string}`; // 1st Trailing punctuation
  chs_trailing_punct2: `${string}`; // 2nd Trailing punctuation
  outlines_odd: `${string}`; // Non standard number of outlines
  outlines_2: `${string}`; // Non standard number of outlines
  numeric_punctuation: `${string}`; // Punct. chs expected WITHIN numbers
  unrecognised_char: `${string}`; // Output char for unidentified blobs
  ok_repeated_ch_non_alphanum_wds: `${string}`; // Allow NN to unrej
  conflict_set_I_l_1: `${string}`; // Il1 conflict set
  file_type: `${string}`; // Filename extension
  tessedit_load_sublangs: `${string}`; // of languages to load with this one
  page_separator: `${string}`; // Page separator (default is form feed control character)
  classify_char_norm_range: `${number}`; // Character Normalization Range ...
  classify_max_rating_ratio: `${number}`; // Veto ratio between classifier ratings
  classify_max_certainty_margin: `${number}`; // Veto difference between classifier certainties
  matcher_good_threshold: `${number}`; // Good Match (0-1)
  matcher_reliable_adaptive_result: `${0 | 1}`; // Great Match (0-1)
  matcher_perfect_threshold: `${number}`; // Perfect Match (0-1)
  matcher_bad_match_pad: `${number}`; // Bad Match Pad (0-1)
  matcher_rating_margin: `${number}`; // New template margin (0-1)
  matcher_avg_noise_size: `${number}`; // Avg. noise blob length
  matcher_clustering_max_angle_delta: `${number}`; // Maximum angle delta for prototype clustering
  classify_misfit_junk_penalty: `${0 | 1}`; // Penalty to apply when a non-alnum is vertically out of its expected textline position
  rating_scale: `${number}`; // Rating scaling factor
  tessedit_class_miss_scale: `${number}`; // Scale factor for features not used
  classify_adapted_pruning_factor: `${number}`; // Prune poor adapted results this much worse than best result
  classify_adapted_pruning_threshold: `${number}`; // Threshold at which classify_adapted_pruning_factor starts
  classify_character_fragments_garbage_certainty_threshold: `${number}`; // Exclude fragments that do not look like whole characters from training and adaption
  speckle_large_max_size: `${number}`; // Max large speckle size
  speckle_rating_penalty: `${number}`; // Penalty to add to worst rating for noise
  xheight_penalty_subscripts: `${number}`; // Score penalty (0.1 = 10%) added if there are subscripts or superscripts in a word, but it is otherwise OK.
  xheight_penalty_inconsistent: `${number}`; // Score penalty (0.1 = 10%) added if an xheight is inconsistent.
  segment_penalty_dict_frequent_word: `${0 | 1}`; // Score multiplier for word matches which have good case and are frequent in the given language (lower is better).
  segment_penalty_dict_case_ok: `${number}`; // Score multiplier for word matches that have good case (lower is better).
  segment_penalty_dict_case_bad: `${number}`; // Default score multiplier for word matches, which may have case issues (lower is better).
  segment_penalty_dict_nonword: `${number}`; // Score multiplier for glyph fragment segmentations which do not match a dictionary word (lower is better).
  segment_penalty_garbage: `${number}`; // Score multiplier for poorly cased strings that are not in the dictionary and generally look like garbage (lower is better).
  certainty_scale: `${number}`; // Certainty scaling factor
  stopper_nondict_certainty_base: `${number}`; // Certainty threshold for non-dict words
  stopper_phase2_certainty_rejection_offset: `${0 | 1}`; // Reject certainty offset
  stopper_certainty_per_char: `${number}`; // Certainty to add for each dict char above small word size.
  stopper_allowable_character_badness: `${number}`; // Max certainty variation allowed in a word (in sigma)
  doc_dict_pending_threshold: `${0 | 1}`; // Worst certainty for using pending dictionary
  doc_dict_certainty_threshold: `${number}`; // Worst certainty for words that can be inserted into the document dictionary
  tessedit_certainty_threshold: `${number}`; // Good blob limit
  chop_split_dist_knob: `${number}`; // Split length adjustment
  chop_overlap_knob: `${number}`; // Split overlap adjustment
  chop_center_knob: `${number}`; // Split center adjustment
  chop_sharpness_knob: `${number}`; // Split sharpness adjustment
  chop_width_change_knob: `${number}`; // Width change adjustment
  chop_ok_split: `${number}`; // OK split limit
  chop_good_split: `${number}`; // Good split limit
  segsearch_max_char_wh_ratio: `${number}`; // Maximum character width-to-height ratio
  language_model_ngram_small_prob: `${number}`; // To avoid overly small denominators use this as the floor of the probability returned by the ngram model.
  language_model_ngram_nonmatch_score: `${number}`; // Average classifier score of a non-matching unichar.
  language_model_ngram_scale_factor: `${number}`; // Strength of the character ngram model relative to the character classifier
  language_model_ngram_rating_factor: `${number}`; // Factor to bring log-probs into the same range as ratings when multiplied by outline length
  language_model_penalty_non_freq_dict_word: `${number}`; // Penalty for words not in the frequent word dictionary
  language_model_penalty_non_dict_word: `${number}`; // Penalty for non-dictionary words
  language_model_penalty_punc: `${number}`; // Penalty for inconsistent punctuation
  language_model_penalty_case: `${number}`; // Penalty for inconsistent case
  language_model_penalty_script: `${number}`; // Penalty for inconsistent script
  language_model_penalty_chartype: `${number}`; // Penalty for inconsistent character type
  language_model_penalty_font: `${0 | 1}`; // Penalty for inconsistent font
  language_model_penalty_spacing: `${number}`; // Penalty for inconsistent spacing
  language_model_penalty_increment: `${number}`; // Penalty increment
  invert_threshold: `${number}`; // For lines with a mean confidence below this value, OCR is also tried with an inverted image
  thresholding_window_size: `${number}`; // Window size for measuring local statistics (to be multiplied by image DPI). This parameter is used by the Sauvola thresholding method
  thresholding_kfactor: `${number}`; // Factor for reducing threshold due to variance. This parameter is used by the Sauvola thresholding method. Normal range: 0.2-0.5
  thresholding_tile_size: `${number}`; // Desired tile size (to be multiplied by image DPI). This parameter is used by the LeptonicaOtsu thresholding method
  thresholding_smooth_kernel_size: `${0 | 1}`; // Size of convolution kernel applied to threshold array (to be multiplied by image DPI). Use 0 for no smoothing. This parameter is used by the LeptonicaOtsu thresholding method
  thresholding_score_fraction: `${number}`; // Fraction of the max Otsu score. This parameter is used by the LeptonicaOtsu thresholding method. For standard Otsu use 0.0, otherwise 0.1 is recommended
  noise_cert_basechar: `${number}`; // Hingepoint for base char certainty
  noise_cert_disjoint: `${number}`; // Hingepoint for disjoint certainty
  noise_cert_punc: `${number}`; // Threshold for new punc char certainty
  noise_cert_factor: `${number}`; // Scaling on certainty diff from Hingepoint
  quality_rej_pc: `${number}`; // good_quality_doc lte rejection limit
  quality_blob_pc: `${0 | 1}`; // good_quality_doc gte good blobs limit
  quality_outline_pc: `${0 | 1}`; // good_quality_doc lte outline error limit
  quality_char_pc: `${number}`; // good_quality_doc gte good char limit
  test_pt_x: `${number}`; // xcoord
  test_pt_y: `${number}`; // ycoord
  tessedit_reject_doc_percent: `${number}`; // %rej allowed before rej whole doc
  tessedit_reject_block_percent: `${number}`; // %rej allowed before rej whole block
  tessedit_reject_row_percent: `${number}`; // %rej allowed before rej whole row
  tessedit_whole_wd_rej_row_percent: `${number}`; // Number of row rejects in whole word rejects which prevents whole row rejection
  tessedit_good_doc_still_rowrej_wd: `${number}`; // rej good doc wd if more than this fraction rejected
  quality_rowrej_pc: `${number}`; // good_quality_doc gte good char limit
  crunch_terrible_rating: `${number}`; // crunch rating lt this
  crunch_poor_garbage_cert: `${number}`; // crunch garbage cert lt this
  crunch_poor_garbage_rate: `${number}`; // crunch garbage rating lt this
  crunch_pot_poor_rate: `${number}`; // POTENTIAL crunch rating lt this
  crunch_pot_poor_cert: `${number}`; // POTENTIAL crunch cert lt this
  crunch_del_rating: `${number}`; // POTENTIAL crunch rating lt this
  crunch_del_cert: `${number}`; // POTENTIAL crunch cert lt this
  crunch_del_min_ht: `${number}`; // Del if word ht lt xht x this
  crunch_del_max_ht: `${number}`; // Del if word ht gt xht x this
  crunch_del_min_width: `${number}`; // Del if word width lt xht x this
  crunch_del_high_word: `${number}`; // Del if word gt xht x this above bl
  crunch_del_low_word: `${number}`; // Del if word gt xht x this below bl
  crunch_small_outlines_size: `${number}`; // Small if lt xht x this
  fixsp_small_outlines_size: `${number}`; // Small if lt xht x this
  superscript_worse_certainty: `${number}`; // How many times worse certainty does a superscript position glyph need to be for us to try classifying it as a char with a different baseline?
  superscript_bettered_certainty: `${number}`; // What reduction in badness do we think sufficient to choose a superscript over what we'd thought. For example, a value of 0.6 means we want to reduce badness of certainty by at least 40%
  superscript_scaledown_ratio: `${number}`; // A superscript scaled down more than this is unbelievably small. For example, 0.3 means we expect the font size to be no smaller than 30% of the text line font size.
  subscript_max_y_top: `${number}`; // Maximum top of a character measured as a multiple of x-height above the baseline for us to reconsider whether it's a subscript.
  superscript_min_y_bottom: `${number}`; // Minimum bottom of a character measured as a multiple of x-height above the baseline for us to reconsider whether it's a superscript.
  suspect_rating_per_ch: `${number}`; // Don't touch bad rating limit
  suspect_accept_rating: `${number}`; // Accept good rating limit
  tessedit_lower_flip_hyphen: `${number}`; // Aspect ratio dot/hyphen test
  tessedit_upper_flip_hyphen: `${number}`; // Aspect ratio dot/hyphen test
  rej_whole_of_mostly_reject_word_fract: `${number}`; // if >this fract
  min_orientation_margin: `${number}`; // Min acceptable orientation margin
  textord_tabfind_vertical_text_ratio: `${number}`; // Fraction of textlines deemed vertical to use vertical page mode
  textord_tabfind_aligned_gap_fraction: `${number}`; // Fraction of height used as a minimum gap for aligned blobs.
  lstm_rating_coefficient: `${number}`; // Sets the rating coefficient for the lstm choices. The smaller the coefficient, the better are the ratings for each choice and less information is lost due to the cut off at 0. The standard value is 5
  tosp_old_sp_kn_th_factor: `${number}`; // Factor for defining space threshold in terms of space and kern sizes
  tosp_threshold_bias1: `${0 | 1}`; // how far between kern and space?
  tosp_threshold_bias2: `${0 | 1}`; // how far between kern and space?
  tosp_narrow_fraction: `${number}`; // Fract of xheight for narrow
  tosp_narrow_aspect_ratio: `${number}`; // narrow if w/h less than this
  tosp_wide_fraction: `${number}`; // Fract of xheight for wide
  tosp_wide_aspect_ratio: `${0 | 1}`; // wide if w/h less than this
  tosp_fuzzy_space_factor: `${number}`; // Fract of xheight for fuzz sp
  tosp_fuzzy_space_factor1: `${number}`; // Fract of xheight for fuzz sp
  tosp_fuzzy_space_factor2: `${number}`; // Fract of xheight for fuzz sp
  tosp_gap_factor: `${number}`; // gap ratio to flip sp->kern
  tosp_kern_gap_factor1: `${number}`; // gap ratio to flip kern->sp
  tosp_kern_gap_factor2: `${number}`; // gap ratio to flip kern->sp
  tosp_kern_gap_factor3: `${number}`; // gap ratio to flip kern->sp
  tosp_ignore_big_gaps: `${number}`; // xht multiplier
  tosp_ignore_very_big_gaps: `${number}`; // xht multiplier
  tosp_rep_space: `${number}`; // rep gap multiplier for space
  tosp_enough_small_gaps: `${number}`; // Fract of kerns reqd for isolated row stats
  tosp_table_kn_sp_ratio: `${number}`; // Min difference of kn & sp in table
  tosp_table_xht_sp_ratio: `${number}`; // Expect spaces bigger than this
  tosp_table_fuzzy_kn_sp_ratio: `${number}`; // Fuzzy if less than this
  tosp_fuzzy_kn_fraction: `${number}`; // New fuzzy kn alg
  tosp_fuzzy_sp_fraction: `${number}`; // New fuzzy sp alg
  tosp_min_sane_kn_sp: `${number}`; // Don't trust spaces less than this time kn
  tosp_init_guess_kn_mult: `${number}`; // Thresh guess - mult kn by this
  tosp_init_guess_xht_mult: `${number}`; // Thresh guess - mult xht by this
  tosp_max_sane_kn_thresh: `${number}`; // Multiplier on kn to limit thresh
  tosp_flip_caution: `${0 | 1}`; // Don't autoflip kn to sp when large separation
  tosp_large_kerning: `${number}`; // Limit use of xht gap with large kns
  tosp_dont_fool_with_small_kerns: `${number}`; // Limit use of xht gap with odd small kns
  tosp_near_lh_edge: `${0 | 1}`; // Don't reduce box if the top left is non blank
  tosp_silly_kn_sp_gap: `${number}`; // Don't let sp minus kn get too small
  tosp_pass_wide_fuzz_sp_to_context: `${number}`; // How wide fuzzies need context
  textord_noise_area_ratio: `${number}`; // Fraction of bounding box for noise
  textord_initialx_ile: `${number}`; // Ile of sizes for xheight guess
  textord_initialasc_ile: `${number}`; // Ile of sizes for xheight guess
  textord_noise_sizelimit: `${number}`; // Fraction of x for big t count
  textord_noise_normratio: `${number}`; // Dot to norm ratio for deletion
  textord_noise_syfract: `${number}`; // xh fract height error for norm blobs
  textord_noise_sxfract: `${number}`; // xh fract width error for norm blobs
  textord_noise_hfract: `${number}`; // Height fraction to discard outlines as speckle noise
  textord_noise_rowratio: `${number}`; // Dot to norm ratio for deletion
  textord_blshift_maxshift: `${0 | 1}`; // Max baseline shift
  textord_blshift_xfraction: `${number}`; // Min size of baseline shift
};

type InitOnlyConfigurationVariableNames =
  | "ambigs_debug_level"
  | "language_model_ngram_on"
  | "language_model_use_sigmoidal_certainty"
  | "load_bigram_dawg"
  | "load_freq_dawg"
  | "load_number_dawg"
  | "load_punc_dawg"
  | "load_system_dawg"
  | "load_unambig_dawg"
  | "tessedit_init_config_only"
  | "tessedit_ocr_engine_mode"
  | "user_patterns_suffix"
  | "user_words_suffix";

export type InitOnlyConfigurationVariables = Pick<
  ConfigurationVariables,
  InitOnlyConfigurationVariableNames
>;
export type SetVariableConfigVariables = Omit<
  ConfigurationVariables,
  InitOnlyConfigurationVariableNames
>;

/**
 * Tesseract init options
 */
export interface TesseractInitOptions {
  /**
   * Its generally safer to use as few languages as possible.
   * The more languages Tesseract needs to load the longer it takes to recognize a image.
   * @public
   */
  lang?: Language[];

  /**
   * OCR Engine Modes
   * The engine mode cannot be changed after creating the instance
   * If another mode is needed, its advised to create a new instance.
   * @throws {Error} Will throw an error when oem mode is below 0 or over 3
   */
  oem?: OcrEngineMode;
  setOnlyNonDebugParams?: boolean;
  configs?: Array<string>;

  vars?: Partial<
    Record<
      keyof ConfigurationVariables,
      ConfigurationVariables[keyof ConfigurationVariables]
    >
  >;
}

export interface TesseractSetRectangleOptions {
  top: number;
  left: number;
  width: number;
  height: number;
}

export interface ProgressChangedInfo {
  /**
   * Chars in this buffer
   */
  progress: number;

  /**
   * Percent complete increasing (0-100)
   */
  percent: number;

  /**
   * States if the worker is still alive
   */
  ocrAlive: number;

  /**
   * top coordinate of the bbox of the current element that tesseract is processing
   */
  top: number;

  /**
   * right coordinate of the bbox of the current element that tesseract is processing
   */
  right: number;

  /**
   * bottom coordinate of the bbox of the current element that tesseract is processing
   */
  bottom: number;

  /**
   * left coordinate of the bbox of the current element that tesseract is processing
   */
  left: number;
}

export interface DetectOrientationScriptResult {
  /**
   * Orientation of the source image in degrees
   * Orientation refers to the way the source is rotated, **not** how the text is
   * aligned. It ranges from 0° to 360° degrees.
   * @type {number}
   */
  orientationDegrees: number;
  /**
   * The confidence of tesseract for the orientation
   * @type {number}
   */
  orientationConfidence: number;

  /**
   * The name of the script that is used in the source image
   * @type {string}
   */
  scriptName: string;

  /**
   * The confidence of tesseract about the detected script of the source image
   * @type {number}
   */
  scriptConfidence: number;
}

export interface TesseractInstance {
  /**
   * Initialize the engine with the given options.
   * @param   {TesseractInitOptions} options Initialization options (languages, datapath, engine mode, etc.).
   * @returns {Promise<void>}
   */
  init(options: TesseractInitOptions): Promise<void>;

  /**
   * Initialize the engine for page analysis only.
   * @returns {Promise<void>}
   */
  initForAnalysePage(): Promise<void>;

  /**
   * Run page layout analysis.
   * @param   {boolean}       mergeSimilarWords Whether to merge similar words during analysis.
   * @returns {Promise<void>}
   */
  analysePage(mergeSimilarWords: boolean): Promise<void>; // TODO: return pageiterator here

  /**
   * Set the page segmentation mode (PSM).
   * @param   {PageSegmentationMode} psm Page segmentation mode.
   * @returns {Promise<void>}
   */
  setPageMode(psm: PageSegmentationMode): Promise<void>;

  /**
   * Set a configuration variable.
   * @param {keyof SetVariableConfigVariables} name Variable name.
   * @param {SetVariableConfigVariables[keyof SetVariableConfigVariables]} value Variable value.
   * @returns Returns `false` if the lookup failed.
   */
  setVariable(
    name: keyof SetVariableConfigVariables,
    value: SetVariableConfigVariables[keyof SetVariableConfigVariables],
  ): Promise<boolean>;

  /**
   * Get a configuration variable as integer.
   * @param   {keyof SetVariableConfigVariables} name Variable name.
   * @returns {Promise<number>} Returns the value of the variable.
   */
  getIntVariable(name: keyof SetVariableConfigVariables): Promise<number>;

  /**
   * Get a configuration variable as boolean (0/1).
   * @param   {keyof SetVariableConfigVariables} name Variable name.
   * @returns {Promise<number>} Returns the value of the variable.
   */
  getBoolVariable(name: keyof SetVariableConfigVariables): Promise<number>;

  /**
   * Get a configuration variable as double.
   * @param   {keyof SetVariableConfigVariables} name Variable name.
   * @returns {Promise<number>} Returns the value of the variable.
   */
  getDoubleVariable(name: keyof SetVariableConfigVariables): Promise<number>;

  /**
   * Get a configuration variable as string.
   * @param   {keyof SetVariableConfigVariables} name Variable name.
   * @returns {Promise<string>} Returns the value of the variable.
   */
  getStringVariable(name: keyof SetVariableConfigVariables): Promise<string>;

  /**
   * Set the image to be recognized.
   * @param   {Buffer<ArrayBuffer>} buffer Image data buffer.
   * @returns {Promise<void>}
   */
  setImage(buffer: Buffer<ArrayBuffer>): Promise<void>;

  /**
   * Restrict recognition to a rectangle.
   * @param   {TesseractSetRectangleOptions} options Rectangle options.
   * @returns {Promise<void>}
   */
  setRectangle(options: TesseractSetRectangleOptions): Promise<void>;

  /**
   * Set the source resolution in PPI.
   * @param   {number} ppi Source resolution in PPI.
   * @returns {Promise<void>}
   */
  setSourceResolution(ppi: number): Promise<void>;

  /**
   * @throws  {Error} Will throw an error if the parameter at index 0 is not a function
   * @param   {(info: ProgressChangedInfo) => void} progressCallback Callback will be called to inform the user about progress changes
   * @returns {Promise<void>}
   */
  recognize(
    progressCallback: (info: ProgressChangedInfo) => void,
  ): Promise<void>;

  /**
   * Detect orientation and script (OSD).
   * @returns {Promise<DetectOrientationScriptResult>}
   */
  detectOrientationScript(): Promise<DetectOrientationScriptResult>;

  /**
   * Get mean text confidence.
   * @returns {Promise<number>} Returns the mean text confidence on resolve
   */
  meanTextConf(): Promise<number>;

  /**
   * Get recognized text as UTF-8.
   * @returns {Promise<string>} Returns the recognized test as utf-8 on resolve
   */
  getUTF8Text(): Promise<string>;

  /**
   * Get hOCR output.
   * @param {Function} progressCallback Optional progress callback.
   * @param {number} pageNumber Optional page number (0-based).
   * @returns {Promise<string>} Returns the `hOCR` upon resolve
   */
  getHOCRText(
    progressCallback?: (info: ProgressChangedInfo) => void,
    pageNumber?: number,
  ): Promise<string>;

  /**
   * Get TSV output.
   * @returns {Promise<string>} Returns the `tsv` upon resolve
   */
  getTSVText(): Promise<string>;

  /**
   * Get UNLV output.
   * @returns {Promise<string>} Returns the `unlv` upon resolve
   */
  getUNLVText(): Promise<string>;

  /**
   * Get ALTO XML output.
   * @param   {Function} progressCallback Optional progress callback.
   * @param   {number} pageNumber Optional page number (0-based).
   * @returns {Promise<string>} Returns the `alto` upon resolve
   */
  getALTOText(
    progressCallback?: (info: ProgressChangedInfo) => void,
    pageNumber?: number,
  ): Promise<string>;

  /**
   * Get languages used at initialization.
   * @returns {Promise<Language>} Returns the languages used when init was called
   */
  getInitLanguages(): Promise<Language>;

  /**
   * Get languages currently loaded.
   * @returns {Promise<Language[]>} Returns the languages that were actually loaded by `init`
   */
  getLoadedLanguages(): Promise<Language[]>;

  /**
   * Get available languages from tessdata.
   * NOTE: this only will return anything after `init` was called before with a valid selection of languages
   * @returns {Promise<Language[]>} Returns the languages that are available to tesseract.
   */
  getAvailableLanguages(): Promise<Language[]>;

  /**
   * Clear internal recognition results/state.
   * @returns {Promise<void>}
   */
  clear(): Promise<void>;

  /**
   * Release native resources and destroy the instance.
   * @returns {Promise<void>}
   */
  end(): Promise<void>;
}

export type NativeTesseract = TesseractInstance;
export type TesseractConstructor = new () => TesseractInstance;

const fs = require("node:fs");
const path = require("node:path");

const rootFromSource = path.resolve(__dirname, "../../");
const bindingOptionsFromSource = path.resolve(
  rootFromSource,
  "binding-options.js",
);
const bindingOptionsPath = fs.existsSync(bindingOptionsFromSource)
  ? bindingOptionsFromSource
  : path.resolve(process.cwd(), "binding-options.js");
const prebuildRoot = fs.existsSync(bindingOptionsFromSource)
  ? rootFromSource
  : process.cwd();

const { Tesseract: NativeTesseract } = require("pkg-prebuilds")(
  prebuildRoot,
  require(bindingOptionsPath),
) as { Tesseract: TesseractConstructor };

class Tesseract extends NativeTesseract {
  constructor() {
    super();
  }
  async init(options: TesseractInitOptions) {
    // scan train data for any files
    // check whether the requested langs are available/cached
    // if not
    //   fetch traineddata from cdn
    //     - add .lock file to downloaded file (while downloading, so other instances
    //       can wait on it and dont have to download again)
    //     - place into tesseract standard folder
    //  if available
    //    just go on with the init function of the native addon

    return super.init(options);
  }
}

export { Tesseract, NativeTesseract };
export default Tesseract;
