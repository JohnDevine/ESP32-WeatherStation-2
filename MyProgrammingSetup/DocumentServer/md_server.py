import http.server
import socketserver
import markdown
import os

PORT = 8000

class MDHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Remove leading / from URL path
        path = self.path.lstrip("/")

        if path.endswith(".md") and os.path.isfile(path):
            # Read Markdown file
            with open(path, "r", encoding="utf-8") as f:
                md_text = f.read()

            # Convert to HTML
            html = f"""
            <html>
            <head>
                <meta charset="utf-8">
                <title>{path}</title>
                <style>
                    body {{ max-width: 700px; margin: auto; font-family: sans-serif; padding: 20px; }}
                    pre {{ background: #f4f4f4; padding: 10px; overflow-x: auto; }}
                    code {{ background: #f4f4f4; padding: 2px 4px; }}
                </style>
            </head>
            <body>
                {markdown.markdown(md_text, extensions=["fenced_code", "tables"])}
            </body>
            </html>
            """

            # Send response
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(html.encode("utf-8"))
        else:
            # Default handler for everything else
            super().do_GET()

# Start server
with socketserver.TCPServer(("", PORT), MDHandler) as httpd:
    print(f"Serving on http://localhost:{PORT}")
    httpd.serve_forever()
