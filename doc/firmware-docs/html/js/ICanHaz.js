/*!
ICanHaz.js version 0.9 -- by @HenrikJoreteg
More info at: http://icanhazjs.com
*/
(function ($) {
/*!
  mustache.js -- Logic-less templates in JavaScript

  by @janl (MIT Licensed, https://github.com/janl/mustache.js/blob/master/LICENSE).

  See http://mustache.github.com/ for more info.
*/

/*global jQuery  */
function ICanHaz() {
    var self = this;
    self.VERSION = "0.9";
    self.templates = {};
    self.partials = {};

    // public function for adding templates
    // We're enforcing uniqueness to avoid accidental template overwrites.
    // If you want a different template, it should have a different name.
    self.addTemplate = function (name, templateString) {
      /*
        if (self[name]) throw "Invalid name: " + name + ".";
        if (self.templates[name]) throw "Template \" + name + \" exists";
*/

        self.templates[name] = templateString;
        self[name] = function (data, raw) {
            data = data || {};
            var result = Mustache.to_html(self.templates[name], data, self.partials);
            return raw ? result : $(result);
        };
    };

    // public function for adding partials
    self.addPartial = function (name, templateString) {
        if (self.partials[name]) {
            throw "Partial \" + name + \" exists";
        } else {
            self.partials[name] = templateString;
        }
    };

    // grabs templates from the DOM and caches them.
    // Loop through and add templates.
    // Whitespace at beginning and end of all templates inside <script> tags will
    // be trimmed. If you want whitespace around a partial, add it in the parent,
    // not the partial. Or do it explicitly using <br/> or &nbsp;
    self.grabTemplates = function () {
        $('script[type="text/html"]').each(function (a, b) {
            var script = $((typeof a === 'number') ? b : a), // Zepto doesn't bind this
                text = (''.trim) ? script.html().trim() : $.trim(script.html());

            text = text.replace(/\[\[/g, "{{");
            text = text.replace(/\]\]/g, "}}");

            /* add every template to both templates and partials. */
            try {
                self.addTemplate(script.attr('id'), text);
                self.addPartial(script.attr('id'), text);
            } catch (err) {
//                debugLog("caught " + err + " while adding " + script.attr('id'));
            }
            script.remove();
        });
    };

    // clears all retrieval functions and empties caches
    self.clearAll = function () {
        for (var key in self.templates) {
            delete self[key];
        }
        self.templates = {};
        self.partials = {};
    };

    self.refresh = function () {
        self.clearAll();
        self.grabTemplates();
    };
}

window.ich = new ICanHaz();

// init itself on document ready
$(function () {
    ich.grabTemplates();
});
})(window.jQuery || window.Zepto);
