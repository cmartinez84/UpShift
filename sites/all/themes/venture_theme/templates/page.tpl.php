<?php
/**
 * @file
 * Default theme implementation to display a single Drupal page.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/garland.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 */
?>
<div id="wrap">
  <div id="header-wrap" class="clr fixed-header">
    <header id="header" class="site-header clr container">
      <div id="logo" class="clr">
        <?php if (theme_get_setting('image_logo','venture_theme')): ?>
        <?php if ($logo): ?><div id="site-logo"><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>">
          <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
        </a></div><?php endif; ?>
        <?php else: ?>
        <h2 id="site-name">
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a>
        </h2>
        <?php endif; ?>
      </div>
      <div id="sidr-close"><a href="#sidr-close" class="toggle-sidr-close"></a></div>
      <div id="site-navigation-wrap">
        <a href="#sidr-main" id="navigation-toggle"><span class="fa fa-bars"></span>Menu</a>
        <nav id="site-navigation" class="navigation main-navigation clr" role="navigation">
          <div id="main-menu" class="menu-main-container">
            <?php 
              $main_menu_tree = menu_tree(variable_get('menu_main_links_source', 'main-menu'));
              print drupal_render($main_menu_tree);
            ?>
          </div>
        </nav>
      </div>
    </header>
  </div>

  <?php if ($is_front): ?>
  <?php if (theme_get_setting('slideshow_display','venture_theme')): ?>
  <?php 
    $slide1_head = check_plain(theme_get_setting('slide1_head','venture_theme'));   $slide1_desc = check_plain(theme_get_setting('slide1_desc','venture_theme')); $slide1_url = check_plain(theme_get_setting('slide1_url','venture_theme'));
    $slide2_head = check_plain(theme_get_setting('slide2_head','venture_theme'));   $slide2_desc = check_plain(theme_get_setting('slide2_desc','venture_theme')); $slide2_url = check_plain(theme_get_setting('slide2_url','venture_theme'));
    $slide3_head = check_plain(theme_get_setting('slide3_head','venture_theme'));   $slide3_desc = check_plain(theme_get_setting('slide3_desc','venture_theme')); $slide3_url = check_plain(theme_get_setting('slide3_url','venture_theme'));
  ?>
  <div id="homepage-slider-wrap" class="clr flexslider-container">
    <div id="homepage-slider" class="flexslider">
      <ul class="slides clr">
        <li class="homepage-slider-slide">
          <a href="<?php print url($slide1_url); ?>">
            <div class="homepage-slide-inner container">
              <?php if($slide1_head || $slide1_desc) : ?>
              <div class="homepage-slide-content">
                <div class="homepage-slide-title"><?php print $slide1_head; ?></div>
                <div class="clr"></div>
                <div class="homepage-slide-caption"><?php print $slide1_desc; ?></div>
              </div>
              <?php endif; ?>
            </div>
            <img src="<?php print base_path() . drupal_get_path('theme', 'venture_theme') . '/images/slide-image-1.jpg'; ?>">
          </a>
        </li>
        <li class="homepage-slider-slide">
          <a href="<?php print url($slide2_url); ?>">
            <div class="homepage-slide-inner container">
              <?php if($slide2_head || $slide2_desc) : ?>
              <div class="homepage-slide-content">
                <div class="homepage-slide-title"><?php print $slide2_head; ?></div>
                <div class="clr"></div>
                <div class="homepage-slide-caption"><?php print $slide2_desc; ?></div>
              </div>
              <?php endif; ?>
            </div>
            <img src="<?php print base_path() . drupal_get_path('theme', 'venture_theme') . '/images/slide-image-2.jpg'; ?>">
          </a>
        </li>
        <li class="homepage-slider-slide">
          <a href="<?php print url($slide3_url); ?>">
            <div class="homepage-slide-inner container">
              <?php if($slide3_head || $slide3_desc) : ?>
              <div class="homepage-slide-content">
                <div class="homepage-slide-title"><?php print $slide3_head; ?></div>
                <div class="clr"></div>
                <div class="homepage-slide-caption"><?php print $slide3_desc; ?></div>
              </div>
              <?php endif; ?>
            </div>
            <img src="<?php print base_path() . drupal_get_path('theme', 'venture_theme') . '/images/slide-image-3.jpg'; ?>">
          </a>
        </li>
      </ul>
    </div>
  </div>
  <?php endif; ?>
  <?php endif; ?>


  <?php if ($page['preface_first'] || $page['preface_middle'] || $page['preface_last']  || $page['header']): ?>
  <div id="preface-wrap" class="site-preface clr">
    <div id="preface" class="clr container">
      <?php if ($page['preface_first'] || $page['preface_middle'] || $page['preface_last']): ?>
        <div id="preface-block-wrap" class="clr">
          <?php if($page['preface_first']): ?><div class="span_1_of_3 col col-1 preface-block ">
            <?php print render ($page['preface_first']); ?>
          </div><?php endif; ?>
          <?php if($page['preface_middle']): ?><div class="span_1_of_3 col col-2 preface-block ">
            <?php print render ($page['preface_middle']); ?>
          </div><?php endif; ?>
          <?php if($page['preface_last']): ?><div class="span_1_of_3 col col-3 preface-block ">
            <?php print render ($page['preface_last']); ?>
          </div><?php endif; ?>
        </div>
      <?php endif; ?>
      <?php if ($page['header']): ?>
        <div class="span_1_of_1 col col-1">
          <?php print render($page['header']); ?>
        </div>
      <?php endif; ?>
    </div>
  </div>
  <?php endif; ?>


  <div id="main" class="site-main container clr">
    <?php $sidebarclass = ""; if($page['sidebar_first']) { $sidebarclass="left-content"; } ?>
    <div id="primary" class="content-area clr">
      <section id="content" role="main" class="site-content <?php print $sidebarclass; ?> clr">
        <?php if (theme_get_setting('breadcrumbs')): ?><?php if ($breadcrumb): ?><div id="breadcrumbs"><?php print $breadcrumb; ?></div><?php endif;?><?php endif; ?>
        <?php print $messages; ?>
        <?php if ($page['content_top']): ?><div id="content_top"><?php print render($page['content_top']); ?></div><?php endif; ?>
        <div id="content-wrap">
          <?php print render($title_prefix); ?>
          <?php if ($title): ?><h1 class="page-title"><?php print $title; ?></h1><?php endif; ?>
          <?php print render($title_suffix); ?>
          <?php if (!empty($tabs['#primary'])): ?><div class="tabs-wrapper clr"><?php print render($tabs); ?></div><?php endif; ?>
          <?php print render($page['help']); ?>
          <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>
          <?php print render($page['content']); ?>
        </div>
      </section>

      <?php if ($page['sidebar_first']): ?>
        <aside id="secondary" class="sidebar-container" role="complementary">
         <?php print render($page['sidebar_first']); ?>
        </aside> 
      <?php endif; ?>
    </div>
  </div>

  <?php if ($page['footer_first'] || $page['footer_second'] || $page['footer_third']  || $page['footer']): ?>
  <div id="footer-wrap" class="site-footer clr">
    <div id="footer" class="clr container">
      <?php if ($page['footer_first'] || $page['footer_second'] || $page['footer_third']): ?>
        <div id="footer-block-wrap" class="clr">
          <?php if($page['footer_first']): ?><div class="span_1_of_3 col col-1 footer-block ">
            <?php print render ($page['footer_first']); ?>
          </div><?php endif; ?>
          <?php if($page['footer_second']): ?><div class="span_1_of_3 col col-2 footer-block ">
            <?php print render ($page['footer_second']); ?>
          </div><?php endif; ?>
          <?php if($page['footer_third']): ?><div class="span_1_of_3 col col-3 footer-block ">
            <?php print render ($page['footer_third']); ?>
          </div><?php endif; ?>
        </div>
      <?php endif; ?>
      
      <?php if ($page['footer']): ?>
        <div class="span_1_of_1 col col-1">
          <?php print render($page['footer']); ?>
        </div>
      <?php endif; ?>
    </div>
  </div>
  <?php endif; ?>

  <footer id="copyright-wrap" class="clear">
    <div id="copyright"><?php print t('Copyright'); ?> &copy; <?php echo date("Y"); ?>, <a href="<?php print $front_page; ?>"><?php print $site_name; ?></a>. <?php print t('Theme by'); ?>  <a href="http://www.devsaran.com" title="Devsaran" target="_blank">Devsaran</a>.</div>
  </footer>
</div>