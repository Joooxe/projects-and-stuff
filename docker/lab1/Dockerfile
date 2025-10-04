FROM alpine:3.21

RUN apk update && apk add --no-cache nginx \
    && adduser -D user0 \
    && mkdir -p /usr/share/html /var/log/nginx /var/lib/nginx/tmp \
    && chown -R user0 /usr/share /etc/nginx /var/lib/nginx /var/log/nginx

COPY nginx.conf /etc/nginx/nginx.conf

USER user0

EXPOSE 8069

ENTRYPOINT ["nginx", "-g", "daemon off;"]
