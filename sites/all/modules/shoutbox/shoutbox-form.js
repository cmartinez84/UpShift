(function ($) {
// Redirect on success.
    $.fn.success_javascript_call = function (val) {
        Drupal.shoutbox.success();
    }
    // Hide or do something on error.
    $.fn.error_javascript_call = function (val) {
    }

    Drupal.shoutbox = {};
    Drupal.behaviors.shoutbox = {
        attach: function (context, settings) {
            var bAjaxEnable = $('#shoutbox_ajax').val();

            if (bAjaxEnable) {
                $('#edit-submit').click(function () {
                    if (('#shoutbox-error').length)
                        $('#shoutbox-error').hide('slow');
                });
            }

            //var options = {
            //resetForm: true,
            //beforeSubmit: Drupal.shoutbox.validate,
            //success: Drupal.shoutbox.success
            //};

            // Detect the shout form
            var shoutForm = $('#shoutbox-add-form:not(.shoutbox-processed)');

            if (shoutForm.length) {
                // Set a class to the form indicating that it's been processed
                $(shoutForm).addClass('shoutbox-processed');

                // Add AJAX behavior to the form
                //$(shoutForm).ajaxForm(options);


                // Tell the form that we have Javascript enabled
                $(shoutForm).find('#edit-js').val(1);

                // Close errors if they're clicked
                $('#shoutbox-error').click(function () {
                    $(this).hide('slow');
                });
                //validate on focus out
//                $('#edit-message').focusout(function () {
//                    if ($(this).hasClass('error') && Drupal.shoutbox.validateElements())
//                    {
//                        //$(this).removeClass('error');
//                    }
//                });
                //validate on focus out
//                $('#edit-nick').focusout(function () {
//                    if ($(this).hasClass('error') && Drupal.shoutbox.validateElements())
//                    {
//                        //$(this).removeClass('error');
//                    }
//                });

                // Show the admin links on hover
                Drupal.shoutbox.adminHover();

                // Initialize the timer for shout updates
                Drupal.shoutbox.shoutTimer('start');

                // Move to end of shoutbox
                /* var elem = document.getElementById('shoutbox-body');
                 elem.scrollTop = elem.scrollHeight;*/
            }
        }
    };

    /**
     * Attach a hover event to the shoutbox admin links
     */
    Drupal.shoutbox.adminHover = function () {
        // Remove binded events
        $('#shoutbox-body .shoutbox-msg').unbind();

        // Bind hover event on admin links
        $('#shoutbox-body .shoutbox-msg').hover(function () {
            $(this).find('.shoutbox-admin-links').show();
        }, function () {
            $(this).find('.shoutbox-admin-links').hide();
        });
    }

    /**
     * Fix the destination query on shout admin link URLs
     *
     * This is required because on an AJAX reload of shouts, the
     * ?destination= query on shout admin links points to the JS
     * callback URL
     */
    Drupal.shoutbox.adminFixDestination = function () {
        var href = '';
        $('.shoutbox-admin-links').find('a').each(function () {
            // Extract the current href
            href = $(this).attr('href');
            // Remove the query
            href = href.substr(0, href.indexOf('?'));
            // Add the new destination
            href = href + '?destination=' + Drupal.settings.shoutbox.currentPath;
            // Set the new href
            $(this).attr('href', href);
        });
    }

    /**
     * Callback for a successful shout submission
     */
    Drupal.shoutbox.success = function (responseText) {
        // Load the latest shouts
        Drupal.shoutbox.loadShouts(true);

        // Move to end of shoutbox
        /* var elem = document.getElementById('shoutbox-body');
         elem.scrollTop = elem.scrollHeight;*/
    }

    /**
     * Starts or stops a timer that triggers every delay seconds.
     */
    Drupal.shoutbox.shoutTimer = function (op) {
        var delay = Drupal.settings.shoutbox.refreshDelay;
        if (delay > 0) {
            switch (op) {
                case 'start':
                    Drupal.shoutbox.interval = setInterval("Drupal.shoutbox.loadShouts()", delay);
                    break;

                case 'stop':
                    clearInterval(Drupal.shoutbox.interval);
                    break;
            }
        }
    }

    /**
     * Reloads all shouts from the server.
     */
    Drupal.shoutbox.loadShouts = function (restoreForm) {
        // Stop the timer
        Drupal.shoutbox.shoutTimer('stop');

        $.ajax({
            url: Drupal.settings.shoutbox.refreshPath,
            type: "GET",
            cache: "false",
            dataType: "json",
            data: {mode: Drupal.settings.shoutbox.mode},
            success: function (response) {
                // Update the shouts
                $("#shoutbox-body").html(response.data);

                // Rebind the hover for admin links
                Drupal.shoutbox.adminHover();

                // Fix the destination URL queries on admin links
                Drupal.shoutbox.adminFixDestination();

                // Resume the timer
                Drupal.shoutbox.shoutTimer('start');

                // Invoke a hook for other modules to act on the added shout.
                if (Drupal.shoutbox.afterPost !== null && Drupal.shoutbox.afterPost !== undefined) {
                    $.each(Drupal.shoutbox.afterPost, function (func) {
                        if ($.isFunction(this.execute)) {
                            this.execute();
                        }
                    });
                }

                // Restore the button
                if (restoreForm) {
                    $('#shoutbox-add-form #edit-message').val('');
                    //$('#shoutbox-throbber').hide();
                    $('#shoutbox-add-form input#edit-submit').show();
                }
            },
            error: function () {
                //$('#shoutbox-throbber').hide();
                $('#shoutbox-error').html(Drupal.t('Error updating shouts. Please refresh the page.'));
                $('#shoutbox-error').show();
                $('#shoutbox-add-form input#edit-submit').show();
            }
        });
    }

    //validate nickname and message
    //    Drupal.shoutbox.validateElements = function ()
    //    {
    //        var errorMsg = '';
    //        // Check if a message is present or is not the default message
    //        if ((!$('#shoutbox-add-form #edit-message').val()) || $('#shoutbox-add-form #edit-message').val() == Drupal.t('Enter Message')) {
    //            errorMsg = Drupal.t('You must enter a message.');
    //            //if message is not valid add error class
    //            $('#shoutbox-add-form #edit-message').addClass('error');
    //        }
    //        else if ((Drupal.settings.shoutbox.maxLength > 0) && ($('#shoutbox-add-form #edit-message').val().length > Drupal.settings.shoutbox.maxLength)) {
    //            errorMsg = Drupal.t('The shout you entered is too long');
    //            //if message is not valid add error class
    //            $('#shoutbox-add-form #edit-message').addClass('error');
    //        }
    //        else
    //            $('#shoutbox-add-form #edit-message').removeClass('error');
    //
    //        var br = '';
    //        if (errorMsg != '')
    //            br = '<br />';
    //        // Check if a nick is present
    //        if ((!$('#shoutbox-add-form #edit-nick').val()) || $('#shoutbox-add-form #edit-nick').val() == Drupal.t('Your Name/Nick')) {
    //            errorMsg += br + Drupal.t('You must enter a nickname.');
    //
    //            //if nickname is not valid add error class
    //            $('#shoutbox-add-form #edit-nick').addClass('error');
    //        }
    //        else
    //            $('#shoutbox-add-form #edit-nick').removeClass('error');
    //
    //        // If a maxlength is set, enforce it
    //        /*if ((Drupal.settings.shoutbox.maxLength > 0) && (nick.value.length > Drupal.settings.shoutbox.maxLength)) {
    //         errorMsg += br + Drupal.t('The nickname you entered is too long');
    //         
    //         //if nickname is not valid add error class
    //         $('#shoutbox-add-form #edit-nick').addClass('error');
    //         }*/
    //
    //        if (errorMsg != '')
    //        {
    //            $('#shoutbox-error').html(errorMsg);
    //            $('#shoutbox-error').show('slow');
    //        }
    //        else
    //            $('#shoutbox-error').hide('slow');
    //
    //        return errorMsg;
    //    }

    /**
     * Validate input before submitting.
     * Don't accept default values or empty strings.
     */
    //    Drupal.shoutbox.validate = function (formData, jqForm, options) {
    //        //show throbber image
    //        //$('#shoutbox-throbber').show();
    //        //hide shout button to avoid double click action
    //        $('#shoutbox-add-form input#edit-submit').hide();
    //
    //        //Validate message and nickname
    //        var bValidation = Drupal.shoutbox.validateElements();
    //
    //        if (bValidation != '') {
    //            //hide throbber image
    //            //$('#shoutbox-throbber').hide();
    //            //show shout button
    //            $('#shoutbox-add-form input#edit-submit').show();
    //            return false;
    //        }
    //        return true;
    //    };

})(jQuery);
