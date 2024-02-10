import \
    unittest
import \
    requests


class TestServer(
    unittest.TestCase):

    def setUp(
            self):
        # Start the server with the specified config file (nginx.conf)
        # This step might involve starting Nginx or any other server you are using
        pass

    def tearDown(
            self):
        # Stop the server after each test
        # This step might involve stopping Nginx or any other server you are using
        pass

    def test_root(
            self):
        # Test root path (/)
        response = requests.get(
            "http://localhost:4281")
        self.assertEqual(
            response.status_code,
            200)

    def test_loc_1(
            self):
        # Test path /loc_1
        response = requests.get("http://localhost:4281/loc_1", timeout=1)
        self.assertEqual(
            response.status_code,
            301)

    def test_loc_1_with_trailing_slash(
            self):
        # Test path /loc_1/ (with trailing slash)
        response = requests.get(
            "http://localhost:4281/loc_1/")
        self.assertEqual(
            response.status_code,
            200)

    def test_loc_2(
            self):
        # Test path /loc_2
        response = requests.get(
            "http://localhost:4281/loc_2")
        self.assertEqual(
            response.status_code,
            301)

    def test_loc_2_with_trailing_slash(
            self):
        # Test path /loc_2/ (with trailing slash)
        response = requests.get(
            "http://localhost:4281/loc_2/")
        self.assertEqual(
            response.status_code,
            403)

    def test_loc_3(
            self):
        # Test path /loc_3
        response = requests.get(
            "http://localhost:4281/loc_3")
        self.assertEqual(
            response.status_code,
            301)

    def test_loc_3_with_trailing_slash(
            self):
        # Test path /loc_3/ (with trailing slash)
        response = requests.get(
            "http://localhost:4281/loc_3/")
        self.assertEqual(
            response.status_code,
            403)

    def test_loc_4(
            self):
        # Test path /loc_4
        response = requests.get(
            "http://localhost:4281/loc_4")
        self.assertEqual(
            response.status_code,
            301)

    def test_loc_4_with_trailing_slash(
            self):
        # Test path /loc_4/ (with trailing slash)
        response = requests.get(
            "http://localhost:4281/loc_4/")
        self.assertEqual(
            response.status_code,
            403)

    def test_loc_4_index_html(
            self):
        # Test path /loc_4/index.html
        response = requests.get(
            "http://localhost:4281/loc_4/index.html")
        self.assertEqual(
            response.status_code,
            200)

    def test_loc_X(
            self):
        # Test path /loc_X
        response = requests.get(
            "http://localhost:4281/loc_X")
        self.assertEqual(
            response.status_code,
            404)

    def test_loc_X_with_trailing_slash(
            self):
        # Test path /loc_X/ (with trailing slash)
        response = requests.get(
            "http://localhost:4281/loc_X/")
        self.assertEqual(
            response.status_code,
            404)


if __name__ == '__main__':
    unittest.main()

# import \
#     unittest
# import \
#     requests
#
#
# class TestServer(
#     unittest.TestCase):
#
#     def setUp(
#             self):
#         # Start the server with the specified config file (nginx.conf)
#         # This step might involve starting Nginx or any other server you are using
#         pass
#
#     def tearDown(
#             self):
#         # Stop the server after each test
#         # This step might involve stopping Nginx or any other server you are using
#         pass
#
#     def test_root(
#             self):
#         # Test root path (/)
#         response = requests.get(
#             "http://localhost:4281")
#         self.assertEqual(
#             response.status_code,
#             200)
#
#     def test_loc_1(
#             self):
#         # Test path /loc_1
#         response = requests.get(
#             "http://localhost:4281/loc_1")
#         self.assertEqual(
#             response.status_code,
#             301)
#         self.assertEqual(
#             response.headers.get("Location"),
#             "http://localhost:4281/loc_1/")
#
#     def test_loc_1_with_trailing_slash(
#             self):
#         # Test path /loc_1/ (with trailing slash)
#         response = requests.get(
#             "http://localhost:4281/loc_1/")
#         self.assertEqual(
#             response.status_code,
#             200)
#
#     def test_loc_2(
#             self):
#         # Test path /loc_2
#         response = requests.get(
#             "http://localhost:4281/loc_2")
#         self.assertEqual(
#             response.status_code,
#             301)
#         self.assertEqual(
#             response.headers.get("Location"),
#             "http://localhost:4281/loc_2/"
#         )
#
#     def test_loc_2_with_trailing_slash(
#             self):
#         # Test path /loc_2/ (with trailing slash)
#         response = requests.get(
#             "http://localhost:4281/loc_2/")
#         self.assertEqual(
#             response.status_code,
#             403)
#
#     def test_loc_3(
#             self):
#         # Test path /loc_3
#         response = requests.get(
#             "http://localhost:4281/loc_3")
#         self.assertEqual(
#             response.status_code,
#             301)
#         self.assertEqual(
#             response.headers.get("Location"),
#             "http://localhost:4281/loc_3/"
#         )
#
#     def test_loc_3_with_trailing_slash(
#             self):
#         # Test path /loc_3/ (with trailing slash)
#         response = requests.get(
#             "http://localhost:4281/loc_3/")
#         self.assertEqual(
#             response.status_code,
#             403)
#
#     def test_loc_4(
#             self):
#         # Test path /loc_4
#         response = requests.get(
#             "http://localhost:4281/loc_4")
#         self.assertEqual(
#             response.status_code,
#             301)
#         self.assertEqual(
#             response.headers.get("Location"),
#             "http://localhost:4281/loc_4/"
#         )
#
#     def test_loc_4_with_trailing_slash(
#             self):
#         # Test path /loc_4/ (with trailing slash)
#         response = requests.get(
#             "http://localhost:4281/loc_4/")
#         self.assertEqual(
#             response.status_code,
#             403)
#
#     def test_loc_4_index_html(
#             self):
#         # Test path /loc_4/index.html
#         response = requests.get(
#             "http://localhost:4281/loc_4/index.html")
#         self.assertEqual(
#             response.status_code,
#             200)
#
#     def test_loc_X(
#             self):
#         # Test path /loc_X
#         response = requests.get(
#             "http://localhost:4281/loc_X")
#         self.assertEqual(
#             response.status_code,
#             404)
#
#     def test_loc_X_with_trailing_slash(
#             self):
#         # Test path /loc_X/ (with trailing slash)
#         response = requests.get(
#             "http://localhost:4281/loc_X/")
#         self.assertEqual(
#             response.status_code,
#             404)
#
#
# if __name__ == '__main__':
#     unittest.main()
