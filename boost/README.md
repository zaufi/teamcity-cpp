Boost listener for TeamCity
---------------------------

To report your test results to TeamCity server include `teamcity_messages.*` and `teamcity_boost.cpp` to your project.
`teamcity_boost.cpp` register a global fixture to replace output formatter if run under TeamCity.
If you have tests with test parameters, see `PARAM_TEST_CASES.txt` for quick solution.


Technical details
-----------------

Reporting is implemented by writing TeamCity service messages to stdout.

See http://www.jetbrains.net/confluence/display/TCD3/Build+Script+Interaction+with+TeamCity for more details.


Contact information
-------------------

http://www.jetbrains.com/support/teamcity


License
-------

Apache, version 2.0
http://www.apache.org/licenses/LICENSE-2.0


TODO
----

* Use CMake > 3.5 and imported targets from `Boost`
* Proper name (w/ a namespace) for exported target
